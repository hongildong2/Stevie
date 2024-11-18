#include "pch.h"

#include "D3D11Resources.h"
#include "D3D11Texture.h"
#include "D3D11Renderer.h"
#include "D3DUtil.h"
#include "D3D11MeshGeometry.h"
#include "Core/Components/MeshComponent.h"
#include "Core/Skybox.h"
#include "Core/Camera.h"
#include "Core/Light.h"

D3D11Renderer::D3D11Renderer()
	: m_deviceResources()
	, m_resourceManager()
	, m_postProcess()
	, m_renderItemIndex(0)
	, m_renderItems{ }
	, m_HDRRenderTarget()
	, m_dwBackBufferWidth(0)
	, m_dwBackBufferHeight(0)
	, m_camera(nullptr)
	, m_skybox(nullptr)
	, m_sunLight(nullptr)
	, m_sunShadowMap(nullptr)
	, m_irradianceMapTexture(nullptr)
	, m_specularMapTexture(nullptr)
	, m_BRDFMapTexture(nullptr)
	, m_pLightsBuffer()
{
	m_deviceResources = std::make_unique<D3D11DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_R32_TYPELESS);
	m_resourceManager = std::make_unique<D3D11ResourceManager>();
	m_postProcess = std::make_unique<D3D11PostProcess>();

}

BOOL D3D11Renderer::Initialize(BOOL bEnableDebugLayer, BOOL bEnableGBV, const WCHAR* wchShaderPath)
{
	if (!m_deviceResources || !m_deviceResources->GetWindow())
	{
		return FALSE;
	}

	m_shaderPath = std::wstring(wchShaderPath);

	m_deviceResources->CreateDeviceResources();
	m_deviceResources->CreateWindowSizeDependentResources();
	m_resourceManager->Initialize(this);
	m_postProcess->Initialize(this);

	m_resourceManager->CreateConstantBuffer(sizeof(GlobalConstant), nullptr, m_globalCB.GetAddressOf());
	m_resourceManager->CreateConstantBuffer(sizeof(RenderParam), nullptr, m_meshCB.GetAddressOf());
	m_resourceManager->CreateConstantBuffer(sizeof(RenderParam), nullptr, m_materialCB.GetAddressOf());
	m_resourceManager->CreateConstantBuffer(sizeof(LightData), nullptr, m_sunLightCB.GetAddressOf());
	m_resourceManager->CreateConstantBuffer(sizeof(RenderParam), nullptr, m_computeCB.GetAddressOf());

	m_sunShadowMap = m_resourceManager->CreateTextureDepth(renderConfig::LIGHT_DEPTH_MAP_WIDTH, renderConfig::LIGHT_DEPTH_MAP_HEIGHT);

	m_pLightsBuffer = m_resourceManager->CreateStructuredBuffer(sizeof(LightData) * MAX_SCENE_LIGHTS_COUNT, sizeof(LightData), nullptr);

	m_HDRRenderTarget = std::unique_ptr<D3D11TextureRender>(m_resourceManager->CreateTextureRender(DXGI_FORMAT_R16G16B16A16_FLOAT, m_dwBackBufferWidth, m_dwBackBufferHeight));
	return TRUE;
}

void D3D11Renderer::BeginRender()
{
	// Clear
	{
		m_deviceResources->PIXBeginEvent(L"Clear");
		auto* context = m_deviceResources->GetD3DDeviceContext();
		auto* depthStencil = m_deviceResources->GetDepthStencilView();
		auto* backBufferRTV = m_deviceResources->GetRenderTargetView();
		auto* hdrRTV = m_HDRRenderTarget->GetRTV();

		context->ClearRenderTargetView(hdrRTV, Colors::Cyan);
		context->ClearRenderTargetView(backBufferRTV, Colors::Cyan);
		context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


		ID3D11RenderTargetView* rtvs[1] =
		{
			hdrRTV,
		};
		context->OMSetRenderTargets(1, rtvs, depthStencil);

		// Set the viewport.
		auto const viewport = m_deviceResources->GetScreenViewport();
		context->RSSetViewports(1, &viewport);
		m_deviceResources->PIXEndEvent();
	}
}

void D3D11Renderer::Render()
{
	// Update Scene Resources
	{
		UpdateGlobalConstant();
		// Light
		m_resourceManager->UpdateStructuredBuffer(sizeof(LightData), m_lights.size(), m_lights.data(), m_pLightsBuffer);
	}

	RenderSkybox();

	RenderOpaques();

	RenderTransparent();
}



void D3D11Renderer::EndRender()
{
	// Post Process
	auto* backBufferRTV = m_deviceResources->GetRenderTargetView();
	m_postProcess->BeginPostProcess(m_HDRRenderTarget);
	m_postProcess->Process();
	m_postProcess->EndPostProcess(backBufferRTV);

	// Reset Render Queue
	m_renderItemIndex = 0;
}

void D3D11Renderer::Present()
{
	m_deviceResources->Present();
}

BOOL D3D11Renderer::SetWindow(HWND hWnd, DWORD dwBackBufferWidth, DWORD dwBackBufferHeight)
{
	if (!m_deviceResources)
	{
		return FALSE;
	}

	m_deviceResources->SetWindow(hWnd, dwBackBufferWidth, dwBackBufferHeight);
	m_dwBackBufferWidth = dwBackBufferWidth;
	m_dwBackBufferHeight = dwBackBufferHeight;

	return TRUE;
}

BOOL D3D11Renderer::UpdateWindowSize(DWORD dwBackBufferWidth, DWORD dwBackBufferHeight)
{
	if (!m_deviceResources || (m_dwBackBufferWidth == dwBackBufferHeight && m_dwBackBufferHeight == dwBackBufferHeight))
	{
		return FALSE;
	}

	m_dwBackBufferWidth = dwBackBufferWidth;
	m_dwBackBufferHeight = dwBackBufferHeight;

	// TODO :: Re init window dependent resources
	{
		// m_HDRRenderTarget->Initialize(this, m_dwBackBufferWidth, m_dwBackBufferHeight);
		// m_postProcess->Initialize(this);
	}

	return m_deviceResources->WindowSizeChanged(m_dwBackBufferWidth, m_dwBackBufferHeight);
}


RMeshGeometry* D3D11Renderer::CreateMeshGeometry(const void* pInVertexList, const UINT vertexSize, const UINT vertexCount, const void* pInIndexList, const UINT indexSize, const UINT indexCount)
{
	MY_ASSERT(pInVertexList != nullptr && pInIndexList != nullptr);
	return static_cast<RMeshGeometry*>(m_resourceManager->CreateMeshGeometry(pInVertexList, vertexSize, vertexCount, pInIndexList, indexSize, indexCount));
}

RMeshGeometry* D3D11Renderer::CreateBasicMeshGeometry(EBasicMeshGeometry type)
{
	switch (type)
	{
	case EBasicMeshGeometry::QUAD:
		return m_resourceManager->CreateQuad();
	case EBasicMeshGeometry::CUBE:
		return m_resourceManager->CreateCube();
	case EBasicMeshGeometry::SPHERE:
		return m_resourceManager->CreateSphere();
	case EBasicMeshGeometry::TESSELLATED_QUAD:
		return m_resourceManager->CreateTessellatedQuad();
	default:
		MY_ASSERT(FALSE);
		return nullptr;
	}
}

RTexture* D3D11Renderer::CreateTexture2DFromWICFile(const WCHAR* wchFileName)
{
	MY_ASSERT(wchFileName != nullptr);
	return static_cast<RTexture*>(m_resourceManager->CreateTexture2DFromWICFile(wchFileName));
}

RTexture* D3D11Renderer::CreateTexture2DFromDDSFile(const WCHAR* wchFileName)
{
	MY_ASSERT(wchFileName != nullptr);
	return static_cast<RTexture*>(m_resourceManager->CreateTexture2DFromDDSFile(wchFileName));
}

RTexture* D3D11Renderer::CreateTextureCubeFromDDSFile(const WCHAR* wchFileName)
{
	MY_ASSERT(wchFileName != nullptr);
	return static_cast<RTexture*>(m_resourceManager->CreateTextureCubeFromDDSFile(wchFileName));
}

void D3D11Renderer::Submit(const MeshComponent* pInMeshComponent, Matrix worldRow)
{
	if (m_renderItemIndex >= MAX_RENDER_ITEM || false == pInMeshComponent->IsActive())
	{
		return;
	}

	UINT currentIndex = m_renderItemIndex++;
	RenderItem& newRenderItem = m_renderItems[currentIndex];
	newRenderItem.pMeshGeometry = pInMeshComponent->GetMeshGeometry();
	newRenderItem.pMaterial = pInMeshComponent->GetMaterial();
	newRenderItem.pBlendState = nullptr;
	newRenderItem.bIsTransparent = false;
	newRenderItem.bIsOccluder = pInMeshComponent->IsOccluder();


	// Parameter, Must be 16byte aligned
	{
		// TODO :: Add bUseHeightMap
		// HACK :: Mesh Parameter
		MeshConstant meshCB;
		Matrix world = worldRow;
		Matrix worldInverse = world.Invert();
		Matrix worldIT = worldInverse.Transpose();

		meshCB.world = world.Transpose();
		meshCB.worldInv = worldInverse.Transpose();
		meshCB.worldIT = worldIT.Transpose();

		MEMCPY_RENDER_PARAM(&newRenderItem.meshParam, &meshCB);

		// Material Parameter
		if (newRenderItem.pMaterial != nullptr)
		{
			newRenderItem.pMaterial->GetMaterialConstant(&newRenderItem.materialParam);
		}
	}

	// Draw Policy
	{
		if (pInMeshComponent->IsTransparent())
		{
			newRenderItem.bIsTransparent = true;
			newRenderItem.pBlendState = pInMeshComponent->GetBlendState();
			newRenderItem.blendFactor = pInMeshComponent->GetBlendFactor();
		}
	}

}

void D3D11Renderer::Compute(const RComputeShader* pComputeShader, const RTexture** pResults, const UINT resultsCount, const RTexture** pResources, const UINT resourcesCount, const RSamplerState** pSamplerStates, const UINT samplerStatesCount, const RenderParam* alignedComputeParam, const UINT batchX, const UINT batchY, const UINT batchZ)
{
	MY_ASSERT(pComputeShader != nullptr);
	MY_ASSERT( resourcesCount <= MAX_COMPUTE_RESOURCE_COUNT && resultsCount <= MAX_COMPUTE_RESOURCE_COUNT && samplerStatesCount <= MAX_COMPUTE_RESOURCE_COUNT);

	const D3D11ComputeShader* cs = static_cast<const D3D11ComputeShader*>(pComputeShader);

	ID3D11ShaderResourceView* srvs[MAX_COMPUTE_RESOURCE_COUNT] = {};
	ID3D11UnorderedAccessView* uavs[MAX_COMPUTE_RESOURCE_COUNT] = {};
	ID3D11SamplerState* sss[MAX_COMPUTE_RESOURCE_COUNT] = {};

	for (UINT i = 0; i < resultsCount; ++i)
	{
		srvs[i] = static_cast<const D3D11Texture*>(pResources[i])->GetSRVOrNull();
	}

	for (UINT i = 0; i < resourcesCount; ++i)
	{
		uavs[i] = static_cast<const D3D11Texture*>(pResources[i])->GetUAVOrNull();
	}

	for (UINT i = 0; i < samplerStatesCount; ++i)
	{
		sss[i] = static_cast<const D3D11SamplerState*>(pSamplerStates[i])->Get();
	}	
	m_resourceManager->UpdateConstantBuffer(sizeof(RenderParam), alignedComputeParam, m_computeCB.Get());

	auto* pContext = m_deviceResources->GetD3DDeviceContext();
	pContext->CSSetShader(cs->Get(), NULL, NULL);
	pContext->CSSetConstantBuffers(0, 1, m_computeCB.GetAddressOf());
	pContext->CSSetShaderResources(0, resourcesCount, srvs);
	pContext->CSSetUnorderedAccessViews(0, resultsCount, uavs, NULL);
	pContext->CSSetSamplers(0, samplerStatesCount, sss);

	pContext->Dispatch(batchX, batchY, batchZ);

	ZeroMemory(srvs, MAX_COMPUTE_RESOURCE_COUNT * sizeof(ID3D11ShaderResourceView*));
	ZeroMemory(uavs, MAX_COMPUTE_RESOURCE_COUNT * sizeof(ID3D11UnorderedAccessView*));

	// release
	pContext->CSSetShaderResources(0, MAX_COMPUTE_RESOURCE_COUNT, srvs);
	pContext->CSSetUnorderedAccessViews(0, MAX_COMPUTE_RESOURCE_COUNT, uavs, NULL);
}

RTexture* D3D11Renderer::CreateTexture3D(const UINT width, const UINT height, const UINT depth, const UINT count, const DXGI_FORMAT format)
{
	return nullptr;
}

RTexture* D3D11Renderer::CreateTexture2D(const UINT width, const UINT height, const UINT count, const DXGI_FORMAT format)
{
	return nullptr;
}

RTexture* D3D11Renderer::CreateStructuredBuffer(const UINT totalSizeInByte, const UINT elementSizeInByte, const UINT elementCount, const void* pInitData)
{
	MY_ASSERT(totalSizeInByte == elementCount * elementSizeInByte);

	return m_resourceManager->CreateStructuredBuffer(elementSizeInByte, elementCount, pInitData);
}

void D3D11Renderer::SetCamera(const Camera* pCamera)
{
	MY_ASSERT(pCamera != nullptr);

	m_camera = pCamera;
}

void D3D11Renderer::SetSkybox(Skybox* pSkybox)
{
	MY_ASSERT(pSkybox != nullptr);
	m_skybox = pSkybox;
}

void D3D11Renderer::SetSunLight(const Light* pLight)
{
	MY_ASSERT(pLight != nullptr);
	m_sunLight = pLight;
}

void D3D11Renderer::SetIBLTextures(const RTexture* pIrradianceMapTexture, const RTexture* pSpecularMapTexture, const RTexture* pBRDFMapTexture)
{
	MY_ASSERT(pIrradianceMapTexture != nullptr && pSpecularMapTexture != nullptr && pBRDFMapTexture != nullptr);
	MY_ASSERT(pIrradianceMapTexture->GetTextureType() == ETextureType::TEXTURE_CUBE);
	MY_ASSERT(pSpecularMapTexture->GetTextureType() == ETextureType::TEXTURE_CUBE);
	MY_ASSERT(pBRDFMapTexture->GetTextureType() == ETextureType::TEXTURE_2D);

	m_irradianceMapTexture = static_cast<const D3D11TextureCube*>(pIrradianceMapTexture);
	m_specularMapTexture = static_cast<const D3D11TextureCube*>(pSpecularMapTexture);
	m_BRDFMapTexture = static_cast<const D3D11Texture2D*>(pBRDFMapTexture);

}

void D3D11Renderer::AddLight(const Light* pLight)
{
	// TODO :: Distinguish shadowing and non-shadowing light
	if (m_lights.size() >= MAX_SCENE_LIGHTS_COUNT)
	{
		return;
	}

	m_lights.push_back(pLight);
}

void D3D11Renderer::UpdateGlobalConstant()
{
	GlobalConstant globalConstant;
	ZeroMemory(&globalConstant, sizeof(GlobalConstant));


	Matrix viewRow = m_camera->GetViewRowMat();
	Matrix projRow = renderConfig::GetCameraProjRowMat();
	Matrix viewProjRow = viewRow * projRow;

	globalConstant.view = viewRow.Transpose();
	globalConstant.proj = projRow.Transpose();
	globalConstant.viewProj = viewProjRow.Transpose();

	globalConstant.invView = globalConstant.view.Invert();
	globalConstant.invProj = globalConstant.proj.Invert();
	globalConstant.invViewProj = globalConstant.viewProj.Invert();

	globalConstant.eyeWorld = m_camera->GetPosWorld();
	globalConstant.globalTime = 0.f;

	globalConstant.eyeDir = m_camera->GetDirWorld();
	globalConstant.globalLightsCount = m_lights.size();

	globalConstant.nearZ = renderConfig::CAMERA_NEAR_Z;
	globalConstant.farZ = renderConfig::CAMERA_FAR_Z;

	m_resourceManager->UpdateConstantBuffer(sizeof(GlobalConstant), &globalConstant, m_globalCB.Get());

	// Sun Light
	LightData lightData;
	ZeroMemory(&lightData, sizeof(LightData));
	MY_ASSERT(m_sunLight != nullptr);
	m_sunLight->GetLightData(&lightData);

	m_resourceManager->UpdateConstantBuffer(sizeof(LightData), &lightData, m_sunLightCB.Get());
}

void D3D11Renderer::SetPipelineStateByMaterial(const RMaterial* pMaterial)
{
	auto* pContext = m_deviceResources->GetD3DDeviceContext();

	UINT samplerStatesCount = 0;
	UINT texturesCount = 0;
	const RSamplerState* samplerStates[RMaterial::MATERIAL_SAMPLE_STATE_MAX_COUNT] = { NULL, };
	const RTexture* textures[RMaterial::MATERIAL_TEXTURE_MAX_COUNT] = { NULL, };
	pMaterial->GetSamplerStates(samplerStates, &samplerStatesCount);
	pMaterial->GetTextures(textures, &texturesCount);


	// Sampler States
	{
		ID3D11SamplerState* sss[RMaterial::MATERIAL_SAMPLE_STATE_MAX_COUNT] = { NULL, };
		for (UINT i = 0; i < samplerStatesCount; ++i)
		{
			MY_ASSERT(samplerStates[i] != nullptr);
			sss[i] = static_cast<const D3D11SamplerState*>(samplerStates[i])->Get();
		}

		if (samplerStatesCount != 0)
		{
			pContext->PSSetSamplers(0, samplerStatesCount, sss);
		}
	}

	// Constant Buffers
	{
		ID3D11Buffer* cbs[4] = { m_globalCB.Get(), m_meshCB.Get(), m_materialCB.Get(), m_sunLightCB.Get() };
		pContext->PSSetConstantBuffers(0, 4, cbs);
	}



	// Shader Resources
	{
		ID3D11ShaderResourceView* srvs[RMaterial::MATERIAL_TEXTURE_MAX_COUNT] = { NULL, };

		const UINT IBL_TEXTURES_COUNT = 3; 	// TODO :: How to manage shader resource slots

		srvs[0] = m_irradianceMapTexture->GetSRVOrNull();
		srvs[1] = m_specularMapTexture->GetSRVOrNull();
		srvs[2] = m_BRDFMapTexture->GetSRVOrNull();

		for (UINT i = 0; i < texturesCount; ++i)
		{
			if (textures[i] != nullptr)
			{
				srvs[i + IBL_TEXTURES_COUNT] = static_cast<const D3D11Texture*>(textures[i])->GetSRVOrNull();
			}
			else
			{
				srvs[i + IBL_TEXTURES_COUNT] = nullptr;
			}
		}

		if (texturesCount != 0 && IBL_TEXTURES_COUNT != 0)
		{
			pContext->PSSetShaderResources(0, texturesCount + IBL_TEXTURES_COUNT, srvs); // Deferred Shading does not need this!
		}
	}


	const D3D11PixelShader* ps = static_cast<const D3D11PixelShader*>(pMaterial->GetShader());
	if (ps != nullptr)
	{
		pContext->PSSetShader(ps->Get(), nullptr, 0);
	}
}


void D3D11Renderer::RenderSkybox()
{
	if (m_skybox != nullptr)
	{
		m_deviceResources->PIXBeginEvent(L"Sky Box");
		auto* pMC = m_skybox->GetMeshComponent();
		const auto* pRMG = static_cast<const D3D11MeshGeometry*>(pMC->GetMeshGeometry());
		auto* pMat = pMC->GetMaterial();

		auto* pContext = m_deviceResources->GetD3DDeviceContext();

		D3D11InputLayout* samplingIL = static_cast<D3D11InputLayout*>(Graphics::SAMPLING_IL);
		pContext->IASetInputLayout(samplingIL->Get());

		pContext->IASetPrimitiveTopology(DX::D3D11::GetD3D11TopologyType(pRMG->GetTopologyType()));
		ID3D11Buffer* pVB[1] = { pRMG->GetVertexBuffer() };
		UINT mVS = pRMG->GetVertexStride();
		UINT mVO = pRMG->GetVertexOffset();
		pContext->IASetVertexBuffers(0, 1, pVB, &mVS, &mVO);
		pContext->IASetIndexBuffer(pRMG->GetIndexBuffer(), pRMG->GetIndexFormat(), 0);


		ID3D11Buffer* cbs[1] = { m_globalCB.Get() };
		D3D11VertexShader* cubemapVS = static_cast<D3D11VertexShader*>(Graphics::CUBEMAP_VS);
		pContext->VSSetShader(cubemapVS->Get(), nullptr, 0);
		pContext->VSSetConstantBuffers(0, 1, cbs);

		SetPipelineStateByMaterial(pMat);

		D3D11RasterizerState* basicRS = static_cast<D3D11RasterizerState*>(Graphics::SOLID_CCW_RS);
		pContext->RSSetState(basicRS->Get());

		pContext->DrawIndexed(pRMG->GetIndexCount(), 0, 0);

		m_deviceResources->PIXEndEvent();
	}
}

void D3D11Renderer::RenderOpaques()
{
	m_deviceResources->PIXBeginEvent(L"Render Opaques");
	for (UINT i = 0; i < m_renderItemIndex; ++i)
	{
		RenderItem& opaqueItem = m_renderItems[i];
		if (opaqueItem.bIsTransparent == true)
		{
			continue;
		}

		switch (opaqueItem.pMeshGeometry->GetMeshType())
		{
		case EMeshType::BASIC:
			Draw(opaqueItem);
			break;
		case EMeshType::TESSELLATED_QUAD:
			DrawTessellatedQuad(opaqueItem);
			break;
		default:
			MY_ASSERT(FALSE);
			break;
		}
	}

	m_deviceResources->PIXEndEvent();
}

void D3D11Renderer::RenderTransparent()
{
	m_deviceResources->PIXBeginEvent(L"Render Transparent");
	for (UINT i = 0; i < m_renderItemIndex; ++i)
	{
		RenderItem& transparentItem = m_renderItems[i];
		if (transparentItem.bIsTransparent == false)
		{
			continue;
		}
		switch (transparentItem.pMeshGeometry->GetMeshType())
		{
		case EMeshType::BASIC:
			Draw(transparentItem);
			break;
		case EMeshType::TESSELLATED_QUAD:
			DrawTessellatedQuad(transparentItem);
			break;
		default:
			MY_ASSERT(FALSE);
			break;
		}
	}

	m_deviceResources->PIXEndEvent();
}

void D3D11Renderer::Draw(const RenderItem& renderItem)
{
	const D3D11MeshGeometry* mesh = static_cast<const D3D11MeshGeometry*>(renderItem.pMeshGeometry);
	const RMaterial* mat = renderItem.pMaterial;
	auto* pContext = m_deviceResources->GetD3DDeviceContext();


	D3D11InputLayout* basicIL = static_cast<D3D11InputLayout*>(Graphics::BASIC_IL);
	pContext->IASetInputLayout(basicIL->Get());
	pContext->IASetPrimitiveTopology(DX::D3D11::GetD3D11TopologyType(mesh->GetTopologyType()));


	ID3D11Buffer* pVB[1] = { mesh->GetVertexBuffer() };
	UINT mVS = mesh->GetVertexStride();
	UINT mVO = mesh->GetVertexOffset();
	pContext->IASetVertexBuffers(0, 1, pVB, &mVS, &mVO);
	pContext->IASetIndexBuffer(mesh->GetIndexBuffer(), mesh->GetIndexFormat(), 0);

	D3D11VertexShader* basicVS = static_cast<D3D11VertexShader*>(Graphics::BASIC_VS);
	pContext->VSSetShader(basicVS->Get(), nullptr, 0);


	if (mat->IsHeightMapped()) // Set HeightMap Resource
	{
		const RTexture* heightMapResources[RMaterial::MATERIAL_TEXTURE_MAX_COUNT] = {};
		UINT heightMapCount = 0;
		mat->GetHeightMapTextures(heightMapResources, &heightMapCount);

		MY_ASSERT(heightMapCount == 1); // BASIC_VS only 1 height map texture;
		MY_ASSERT(heightMapResources[0] != nullptr);

		ID3D11ShaderResourceView* srv[1] = { static_cast<const D3D11Texture*>(heightMapResources[0])->GetSRVOrNull() };
		pContext->VSSetShaderResources(0, 1, srv);
	}

	m_resourceManager->UpdateConstantBuffer(sizeof(RenderParam), &renderItem.meshParam, m_meshCB.Get());
	ID3D11Buffer* cbs[2] = { m_globalCB.Get(), m_meshCB.Get() };
	pContext->VSSetConstantBuffers(0, 2, cbs);



	SetPipelineStateByMaterial(mat);
	m_resourceManager->UpdateConstantBuffer(sizeof(RenderParam), &renderItem.materialParam, m_materialCB.Get());
	D3D11RasterizerState* basicRS = static_cast<D3D11RasterizerState*>(Graphics::SOLID_CW_RS);
	pContext->RSSetState(basicRS->Get());

	if (renderItem.bIsTransparent)
	{
		const D3D11BlendState* bs = static_cast<const D3D11BlendState*>(renderItem.pBlendState);
		FLOAT blendFactor[4] = {
			renderItem.blendFactor.x,
			renderItem.blendFactor.y,
			renderItem.blendFactor.z,
			renderItem.blendFactor.w,
		};

		pContext->OMSetBlendState(bs->Get(), blendFactor, 0);
	}


	pContext->DrawIndexed(mesh->GetIndexCount(), 0, 0);

}

void D3D11Renderer::DrawTessellatedQuad(const RenderItem& renderItem)
{
	const D3D11MeshGeometry* mesh = static_cast<const D3D11MeshGeometry*>(renderItem.pMeshGeometry);
	const RMaterial* mat = renderItem.pMaterial;
	auto* pContext = m_deviceResources->GetD3DDeviceContext();


	D3D11InputLayout* basicIL = static_cast<D3D11InputLayout*>(Graphics::BASIC_IL);
	pContext->IASetInputLayout(basicIL->Get());
	pContext->IASetPrimitiveTopology(DX::D3D11::GetD3D11TopologyType(mesh->GetTopologyType()));

	ID3D11Buffer* pVB[1] = { mesh->GetVertexBuffer() };
	UINT mVS = mesh->GetVertexStride();
	UINT mVO = mesh->GetVertexOffset();
	pContext->IASetVertexBuffers(0, 1, pVB, &mVS, &mVO);
	pContext->IASetIndexBuffer(mesh->GetIndexBuffer(), mesh->GetIndexFormat(), 0);

	pContext->VSSetShader(nullptr, 0, 0);
	D3D11DomainShader* ds = static_cast<D3D11DomainShader*>(Graphics::TESSELATED_QUAD_DS);
	pContext->DSSetShader(ds->Get(), 0, 0);
	D3D11HullShader* hs = static_cast<D3D11HullShader*>(Graphics::TESSELLATED_QUAD_HS);
	pContext->HSSetShader(hs->Get(), 0, 0);


	if (mat->IsHeightMapped()) // Set HeightMap Resource
	{
		const RTexture* heightMapResources[RMaterial::MATERIAL_TEXTURE_MAX_COUNT] = {};
		UINT heightMapCount = 0;
		mat->GetHeightMapTextures(heightMapResources, &heightMapCount);

		MY_ASSERT(heightMapCount == 3);
		MY_ASSERT(heightMapResources[0] != nullptr && heightMapResources[1] != nullptr && heightMapResources[2] != nullptr);

		ID3D11ShaderResourceView* srv[3] =
		{
			static_cast<const D3D11Texture*>(heightMapResources[0])->GetSRVOrNull(),
			static_cast<const D3D11Texture*>(heightMapResources[1])->GetSRVOrNull(),
			static_cast<const D3D11Texture*>(heightMapResources[2])->GetSRVOrNull()
		};
		pContext->DSSetShaderResources(0, 3, srv);
	}

	m_resourceManager->UpdateConstantBuffer(sizeof(RenderParam), &renderItem.meshParam, m_meshCB.Get());
	ID3D11Buffer* cbs[2] = { m_globalCB.Get(), m_meshCB.Get() };
	pContext->VSSetConstantBuffers(0, 2, cbs);



	SetPipelineStateByMaterial(mat);
	m_resourceManager->UpdateConstantBuffer(sizeof(RenderParam), &renderItem.materialParam, m_materialCB.Get());
	D3D11RasterizerState* basicRS = static_cast<D3D11RasterizerState*>(Graphics::SOLID_CW_RS);
	pContext->RSSetState(basicRS->Get());

	if (renderItem.bIsTransparent)
	{
		const D3D11BlendState* bs = static_cast<const D3D11BlendState*>(renderItem.pBlendState);
		FLOAT blendFactor[4] = {
			renderItem.blendFactor.x,
			renderItem.blendFactor.y,
			renderItem.blendFactor.z,
			renderItem.blendFactor.w,
		};

		pContext->OMSetBlendState(bs->Get(), blendFactor, 0);
	}


	pContext->DrawIndexed(mesh->GetIndexCount(), 0, 0);
}

