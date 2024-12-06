#include "pch.h"
#include "D3D11Resources.h"
#include "D3D11Texture.h"
#include "D3DUtil.h"
#include "D3D11MeshGeometry.h"
#include "Core/Components/MeshComponent.h"
#include "Core/Skybox.h"
#include "Core/Camera.h"
#include "Core/Light.h"

RRenderer::RRenderer()
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
	, m_irradianceMapTexture(nullptr)
	, m_specularMapTexture(nullptr)
	, m_BRDFMapTexture(nullptr)
	, m_sceneLightsBuffer()
	, m_sceneLights{}
	, m_sceneLightsIndex(0)
{
	m_deviceResources = std::make_unique<D3D11DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_R32_TYPELESS);
	m_resourceManager = std::make_unique<D3D11ResourceManager>();
	m_postProcess = std::make_unique<D3D11PostProcess>();

}

BOOL RRenderer::Initialize(BOOL bEnableDebugLayer, BOOL bEnableGBV, const WCHAR* wchShaderPath)
{
	bEnableDebugLayer;
	bEnableGBV;
	if (!m_deviceResources || !m_deviceResources->GetWindow())
	{
		return FALSE;
	}

	m_shaderPath = std::wstring(wchShaderPath);

	m_deviceResources->CreateDeviceResources();
	m_deviceResources->CreateWindowSizeDependentResources();
	m_resourceManager->Initialize(this);
	m_postProcess->Initialize(this);

	m_resourceManager->CreateConstantBuffer(sizeof(RGlobalConstant), nullptr, m_globalCB.GetAddressOf());
	m_resourceManager->CreateConstantBuffer(sizeof(RLightConstant), nullptr, m_sunLightCB.GetAddressOf());
	m_resourceManager->CreateConstantBuffer(sizeof(RenderParam), nullptr, m_meshCB.GetAddressOf());
	m_resourceManager->CreateConstantBuffer(sizeof(RenderParam), nullptr, m_materialCB.GetAddressOf());
	m_resourceManager->CreateConstantBuffer(sizeof(RenderParam), nullptr, m_computeCB.GetAddressOf());

	m_sceneLightsBuffer = m_resourceManager->CreateStructuredBuffer(sizeof(RLightConstant), MAX_SCENE_LIGHTS_COUNT, nullptr);

	m_HDRRenderTarget = std::unique_ptr<RTexture>(m_resourceManager->CreateTextureRender(DXGI_FORMAT_R16G16B16A16_FLOAT, m_dwBackBufferWidth, m_dwBackBufferHeight));
	return TRUE;
}

void RRenderer::BeginRender()
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

void RRenderer::Render()
{
	// Update Scene Resources
	{
		UpdateGlobalConstant();
		// Light
		m_resourceManager->UpdateStructuredBuffer(sizeof(RLightConstant), static_cast<UINT>(m_sceneLightsIndex), m_sceneLights, m_sceneLightsBuffer);
	}

	RenderSkybox();

	RenderOpaques();

	RenderTransparent();
}



void RRenderer::EndRender()
{
	// Post Process
	auto* backBufferRTV = m_deviceResources->GetRenderTargetView();
	m_postProcess->BeginPostProcess(m_HDRRenderTarget);
	m_postProcess->Process();
	m_postProcess->EndPostProcess(backBufferRTV);

	// Reset Render Queue
	m_renderItemIndex = 0;

	// Reset Scene Lights
	m_sceneLightsIndex = 0;
	m_shadowingLightsIndex = 0;


	// Release
	ID3D11ShaderResourceView* release[20] = { NULL, };
	auto* pContext = m_deviceResources->GetD3DDeviceContext();
	pContext->VSSetShaderResources(0, 20, release);
	pContext->PSSetShaderResources(0, 20, release);
	pContext->DSSetShaderResources(0, 20, release);
	pContext->HSSetShaderResources(0, 20, release);
}

void RRenderer::Present()
{
	m_deviceResources->Present();
}

BOOL RRenderer::SetWindow(HWND hWnd, DWORD dwBackBufferWidth, DWORD dwBackBufferHeight)
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

BOOL RRenderer::UpdateWindowSize(DWORD dwBackBufferWidth, DWORD dwBackBufferHeight)
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


RMeshGeometry* RRenderer::CreateMeshGeometry(const void* pInVertexList, const UINT vertexSize, const UINT vertexCount, const void* pInIndexList, const UINT indexSize, const UINT indexCount, const EPrimitiveTopologyType topologyType, const EMeshType meshType)
{
	MY_ASSERT(pInVertexList != nullptr && pInIndexList != nullptr);
	return static_cast<RMeshGeometry*>(m_resourceManager->CreateMeshGeometry(pInVertexList, vertexSize, vertexCount, pInIndexList, indexSize, indexCount, topologyType, meshType));
}

RMeshGeometry* RRenderer::CreateBasicMeshGeometry(EBasicMeshGeometry type)
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

RTexture* RRenderer::CreateTexture2DFromWICFile(const WCHAR* wchFileName)
{
	MY_ASSERT(wchFileName != nullptr);
	return static_cast<RTexture*>(m_resourceManager->CreateTexture2DFromWICFile(wchFileName));
}

RTexture* RRenderer::CreateTexture2DFromDDSFile(const WCHAR* wchFileName)
{
	MY_ASSERT(wchFileName != nullptr);
	return static_cast<RTexture*>(m_resourceManager->CreateTexture2DFromDDSFile(wchFileName));
}

RTexture* RRenderer::CreateTextureCubeFromDDSFile(const WCHAR* wchFileName)
{
	MY_ASSERT(wchFileName != nullptr);
	return static_cast<RTexture*>(m_resourceManager->CreateTextureCubeFromDDSFile(wchFileName));
}

void RRenderer::Submit(const MeshComponent* pInMeshComponent, Matrix worldRow)
{
	if (m_renderItemIndex >= MAX_RENDER_ITEM || false == pInMeshComponent->IsActive())
	{
		return;
	}

	UINT currentIndex = m_renderItemIndex++;
	RenderItem& newRenderItem = m_renderItems[currentIndex];
	newRenderItem.pMeshGeometry = pInMeshComponent->GetMeshGeometry();


	// Shader Bindings
	const RMaterial* pMat = pInMeshComponent->GetMaterial();
	newRenderItem.pPS = pMat->GetShader();
	newRenderItem.geometrySSCount = pMat->GetGeometrySamplerStatesCount();
	newRenderItem.pixelSSCount = pMat->GetPixelSamplerStatesCount();
	newRenderItem.geometryTexCount = pMat->GetGeometryTexturesCount();
	newRenderItem.pixelTexCount = pMat->GetPixelTexturesCount();

	pMat->GetGeometrySamplerStates(newRenderItem.ppGeometrySamplerStates);
	pMat->GetGeometryTextures(newRenderItem.ppGeometryTextures);
	pMat->GetPixelSamplerStates(newRenderItem.ppPixelSamplerStates);
	pMat->GetPixelTextures(newRenderItem.ppPixelTextures);

	newRenderItem.pBlendState = nullptr;
	newRenderItem.bIsTransparent = false;
	newRenderItem.bIsOccluder = pInMeshComponent->IsOccluder();


	// Parameter, Must be 16byte aligned
	{
		BOOL bIsHeightMapped = newRenderItem.geometryTexCount > 0;
		RMeshConstant meshCB(worldRow, bIsHeightMapped);
		MEMCPY_RENDER_PARAM(&newRenderItem.meshParam, &meshCB);
	}

	if (pMat != nullptr)
	{
		pMat->GetMaterialConstant(&newRenderItem.materialParam);
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

void RRenderer::Compute(const RComputeShader* pComputeShader, const WCHAR* pTaskName, const RTexture** pResults, const UINT resultsCount, const RTexture** pResources, const UINT resourcesCount, const RSamplerState** pSamplerStates, const UINT samplerStatesCount, const RenderParam* pAlignedComputeParam, const UINT batchX, const UINT batchY, const UINT batchZ)
{
	MY_ASSERT(pComputeShader != nullptr);
	MY_ASSERT(resourcesCount <= MAX_COMPUTE_RESOURCE_COUNT && resultsCount <= MAX_COMPUTE_RESOURCE_COUNT && samplerStatesCount <= MAX_COMPUTE_RESOURCE_COUNT);
	const D3D11ComputeShader* cs = static_cast<const D3D11ComputeShader*>(pComputeShader);
	auto* pContext = m_deviceResources->GetD3DDeviceContext();

	m_deviceResources->PIXBeginEvent(pTaskName);

	ID3D11ShaderResourceView* srvs[MAX_COMPUTE_RESOURCE_COUNT] = {};
	ID3D11UnorderedAccessView* uavs[MAX_COMPUTE_RESOURCE_COUNT] = {};
	ID3D11SamplerState* sss[MAX_COMPUTE_RESOURCE_COUNT] = {};

	for (UINT i = 0; i < resourcesCount; ++i)
	{
		srvs[i] = static_cast<const D3D11Texture*>(pResources[i])->GetSRV();
	}

	for (UINT i = 0; i < resultsCount; ++i)
	{
		uavs[i] = static_cast<const D3D11Texture*>(pResults[i])->GetUAV();
	}

	for (UINT i = 0; i < samplerStatesCount; ++i)
	{
		sss[i] = static_cast<const D3D11SamplerState*>(pSamplerStates[i])->Get();
	}



	pContext->CSSetShader(cs->Get(), NULL, NULL);
	pContext->CSSetShaderResources(0, resourcesCount, srvs);
	pContext->CSSetUnorderedAccessViews(0, resultsCount, uavs, NULL);
	pContext->CSSetSamplers(0, samplerStatesCount, sss);

	if (pAlignedComputeParam != nullptr)
	{
		m_resourceManager->UpdateConstantBuffer(sizeof(RenderParam), pAlignedComputeParam, m_computeCB.Get());
		pContext->CSSetConstantBuffers(0, 1, m_computeCB.GetAddressOf());
	}

	pContext->Dispatch(batchX, batchY, batchZ);

	ID3D11ShaderResourceView* releaseSRV[MAX_COMPUTE_RESOURCE_COUNT] = { NULL, };
	ID3D11UnorderedAccessView* releaseUAV[MAX_COMPUTE_RESOURCE_COUNT] = { NULL, };
	// release
	pContext->CSSetShaderResources(0, MAX_COMPUTE_RESOURCE_COUNT, releaseSRV);
	pContext->CSSetUnorderedAccessViews(0, MAX_COMPUTE_RESOURCE_COUNT, releaseUAV, NULL);
	m_deviceResources->PIXEndEvent();
}

RTexture* RRenderer::CreateTexture3D(const UINT width, const UINT height, const UINT depth, const DXGI_FORMAT format)
{
	return m_resourceManager->CreateTexture3D(width, height, depth, format);
}

RTexture* RRenderer::CreateTexture2D(const UINT width, const UINT height, const UINT count, const DXGI_FORMAT format)
{
	return m_resourceManager->CreateTexture2D(width, height, count, format);
}

RTexture* RRenderer::CreateStructuredBuffer(const UINT uElementSize, const UINT uElementCount, const void* pInitData)
{
	return m_resourceManager->CreateStructuredBuffer(uElementSize, uElementCount, pInitData);
}

void RRenderer::SetCamera(const Camera* pCamera)
{
	MY_ASSERT(pCamera != nullptr);

	m_camera = pCamera;
}

void RRenderer::SetSkybox(Skybox* pSkybox)
{
	MY_ASSERT(pSkybox != nullptr);
	m_skybox = pSkybox;
}

void RRenderer::SetSunLight(const Light* pLight)
{
	MY_ASSERT(pLight != nullptr);
	m_sunLight = pLight;
}

void RRenderer::SetIBLTextures(const RTexture* pIrradianceMapTexture, const RTexture* pSpecularMapTexture, const RTexture* pBRDFMapTexture)
{
	MY_ASSERT(pIrradianceMapTexture != nullptr && pSpecularMapTexture != nullptr && pBRDFMapTexture != nullptr);
	MY_ASSERT(pIrradianceMapTexture->GetTextureType() == ETextureType::TEXTURE_CUBE);
	MY_ASSERT(pSpecularMapTexture->GetTextureType() == ETextureType::TEXTURE_CUBE);
	MY_ASSERT(pBRDFMapTexture->GetTextureType() == ETextureType::TEXTURE_2D);

	m_irradianceMapTexture = pIrradianceMapTexture;
	m_specularMapTexture = pSpecularMapTexture;
	m_BRDFMapTexture = pBRDFMapTexture;

}

void RRenderer::AddLight(const Light* pLight)
{
	if (m_sceneLightsIndex >= MAX_SCENE_LIGHTS_COUNT)
	{
		return;
	}

	m_sceneLights[m_sceneLightsIndex] = pLight->GetLightConstant();
	if (pLight->IsShadowingLight() && m_shadowingLightsIndex < MAX_SHADOWING_LIGHTS_COUNT)
	{
		m_shadowingLightsIndices[m_shadowingLightsIndex++] = m_sceneLightsIndex;
	}

	++m_sceneLightsIndex;
}

void RRenderer::UpdateGlobalConstant()
{
	RGlobalConstant globalConstant;
	ZeroMemory(&globalConstant, sizeof(RGlobalConstant));


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
	globalConstant.globalLightsCount = m_sceneLightsIndex;
	globalConstant.shadowingLightsCount = m_shadowingLightsIndex;

	globalConstant.nearZ = renderConfig::CAMERA_NEAR_Z;
	globalConstant.farZ = renderConfig::CAMERA_FAR_Z;

	m_resourceManager->UpdateConstantBuffer(sizeof(RGlobalConstant), &globalConstant, m_globalCB.Get());

	// Sun Light
	RLightConstant lightData = m_sunLight->GetLightConstant();
	m_resourceManager->UpdateConstantBuffer(sizeof(RLightConstant), &lightData, m_sunLightCB.Get());
}

void RRenderer::SetPipelineState(const RenderItem& item)
{
	auto* pContext = m_deviceResources->GetD3DDeviceContext();

	// Geometry Pipeline State
	{
		if (item.geometryTexCount > 0)
		{
			ID3D11ShaderResourceView* srvs[renderLimits::MAX_RENDER_BINDINGS_COUNT] = { NULL, };
			for (UINT i = 0; i < item.geometryTexCount; ++i)
			{
				srvs[i] = static_cast<const D3D11Texture*>(item.ppGeometryTextures[i])->GetSRV();
			}
			pContext->VSSetShaderResources(0, item.geometryTexCount, srvs);
			pContext->DSSetShaderResources(0, item.geometryTexCount, srvs);

		}

		if (item.geometrySSCount > 0)
		{
			ID3D11SamplerState* sss[renderLimits::MAX_RENDER_BINDINGS_COUNT] = { NULL, };
			for (UINT i = 0; i < item.geometrySSCount; ++i)
			{
				sss[i] = item.ppGeometrySamplerStates[i]->Get();
			}
			pContext->VSSetSamplers(0, item.geometrySSCount, sss);
			pContext->DSSetSamplers(0, item.geometrySSCount, sss);
		}
	}


	// Pixel Shader
	if (item.pPS != nullptr)
	{
		pContext->PSSetShader(item.pPS->Get(), nullptr, 0);


		// Sampler States
		if (item.pixelSSCount > 0)
		{
			ID3D11SamplerState* sss[renderLimits::MAX_RENDER_BINDINGS_COUNT] = { NULL, };
			for (UINT i = 0; i < item.pixelSSCount; ++i)
			{
				MY_ASSERT(item.ppPixelSamplerStates[i] != nullptr);
				sss[i] = item.ppPixelSamplerStates[i]->Get();
			}

			pContext->PSSetSamplers(0, item.pixelSSCount, sss);
		}

		// Shader Resources
		ID3D11ShaderResourceView* srvs[renderLimits::MAX_RENDER_BINDINGS_COUNT] = { NULL, };
		if (item.pixelTexCount > 0 && SCENE_RESOURCES_COUNT != 0)
		{
			srvs[0] = m_irradianceMapTexture->GetSRV();
			srvs[1] = m_specularMapTexture->GetSRV();
			srvs[2] = m_BRDFMapTexture->GetSRV();
			srvs[3] = m_sceneLightsBuffer->GetSRV();

			for (UINT i = 0; i < item.pixelTexCount; ++i)
			{
				if (item.ppPixelTextures[i] != nullptr)
				{
					srvs[i + SCENE_RESOURCES_COUNT] = static_cast<const D3D11Texture*>(item.ppPixelTextures[i])->GetSRV();
				}
				else
				{
					srvs[i + SCENE_RESOURCES_COUNT] = nullptr;
				}
			}
			pContext->PSSetShaderResources(0, item.pixelTexCount + SCENE_RESOURCES_COUNT, srvs);
		}
	}





	// Constant Buffers
	{
		ID3D11Buffer* cbs[4] = { m_globalCB.Get(), m_meshCB.Get(), m_materialCB.Get(), m_sunLightCB.Get() };
		pContext->PSSetConstantBuffers(0, 4, cbs);
	}







}


void RRenderer::RenderSkybox()
{
	if (m_skybox != nullptr)
	{
		m_deviceResources->PIXBeginEvent(L"Sky Box");
		auto* pMC = m_skybox->GetMeshComponent();
		const auto* pRMG = pMC->GetMeshGeometry();
		auto* pMat = pMC->GetMaterial();

		auto* pContext = m_deviceResources->GetD3DDeviceContext();

		pContext->IASetInputLayout(Graphics::SAMPLING_IL->Get());

		pContext->IASetPrimitiveTopology(GetD3D11TopologyType(pRMG->GetTopologyType()));
		ID3D11Buffer* pVB[1] = { pRMG->GetVertexBuffer() };
		UINT mVS = pRMG->GetVertexStride();
		UINT mVO = pRMG->GetVertexOffset();
		pContext->IASetVertexBuffers(0, 1, pVB, &mVS, &mVO);
		pContext->IASetIndexBuffer(pRMG->GetIndexBuffer(), pRMG->GetIndexFormat(), 0);


		ID3D11Buffer* cbs[1] = { m_globalCB.Get() };
		pContext->VSSetShader(Graphics::CUBEMAP_VS->Get(), nullptr, 0);
		pContext->VSSetConstantBuffers(0, 1, cbs);

		pContext->PSSetShader(Graphics::CUBEMAP_PS->Get(), 0, 0);
		UINT psSSCount = pMat->GetPixelSamplerStatesCount();
		UINT psTexCount = pMat->GetPixelTexturesCount();

		const RSamplerState* sss[renderLimits::MAX_RENDER_BINDINGS_COUNT] = { NULL, };
		const RTexture* texs[renderLimits::MAX_RENDER_BINDINGS_COUNT] = { NULL, };

		pMat->GetPixelSamplerStates(sss);
		pMat->GetPixelTextures(texs);

		ID3D11SamplerState* d3dsss[renderLimits::MAX_RENDER_BINDINGS_COUNT] = { NULL, };
		for (UINT i = 0; i < psSSCount; ++i)
		{
			MY_ASSERT(sss[i] != nullptr);
			d3dsss[i] = sss[i]->Get();
		}
		ID3D11ShaderResourceView* srvs[renderLimits::MAX_RENDER_BINDINGS_COUNT] = { NULL, };
		for (UINT i = 0; i < psTexCount; ++i)
		{
			MY_ASSERT(texs[i] != nullptr);
			srvs[i] = static_cast<const D3D11Texture*>(texs[i])->GetSRV();
		}

		pContext->PSSetSamplers(0, psSSCount, d3dsss);
		pContext->PSSetShaderResources(0, psTexCount, srvs);


		pContext->RSSetState(Graphics::SOLID_CCW_RS->Get());
		pContext->OMSetDepthStencilState(Graphics::SKYBOX_DSS->Get(), 0);

		pContext->DrawIndexed(pRMG->GetIndexCount(), 0, 0);

		m_deviceResources->PIXEndEvent();
	}
}

void RRenderer::RenderOpaques()
{
	m_deviceResources->PIXBeginEvent(L"Render Opaques");

	auto* pContext = m_deviceResources->GetD3DDeviceContext();
	pContext->OMSetDepthStencilState(Graphics::OPAQUE_DSS->Get(), 0);
	pContext->RSSetState(Graphics::SOLID_CW_RS->Get());

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

void RRenderer::RenderTransparent()
{
	m_deviceResources->PIXBeginEvent(L"Render Transparent");

	auto* pContext = m_deviceResources->GetD3DDeviceContext();
	pContext->OMSetDepthStencilState(Graphics::TRANSPARENT_DSS->Get(), 0);
	pContext->RSSetState(Graphics::SOLID_CW_RS->Get());

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

void RRenderer::Draw(const RenderItem& renderItem)
{
	const RMeshGeometry* mesh = renderItem.pMeshGeometry;
	auto* pContext = m_deviceResources->GetD3DDeviceContext();

	// Geometry Pipeline
	{
		pContext->IASetInputLayout(Graphics::BASIC_IL->Get());
		pContext->IASetPrimitiveTopology(GetD3D11TopologyType(mesh->GetTopologyType()));

		ID3D11Buffer* pVB[1] = { mesh->GetVertexBuffer() };
		UINT mVS = mesh->GetVertexStride();
		UINT mVO = mesh->GetVertexOffset();
		pContext->IASetVertexBuffers(0, 1, pVB, &mVS, &mVO);
		pContext->IASetIndexBuffer(mesh->GetIndexBuffer(), mesh->GetIndexFormat(), 0);
		pContext->VSSetShader(Graphics::BASIC_VS->Get(), nullptr, 0);


		m_resourceManager->UpdateConstantBuffer(sizeof(RenderParam), &renderItem.meshParam, m_meshCB.Get());
		ID3D11Buffer* cbs[2] = { m_globalCB.Get(), m_meshCB.Get() };
		pContext->VSSetConstantBuffers(0, 2, cbs);
	}


	SetPipelineState(renderItem);
	m_resourceManager->UpdateConstantBuffer(sizeof(RenderParam), &renderItem.materialParam, m_materialCB.Get());

	if (renderItem.bIsTransparent)
	{
		const D3D11BlendState* bs = static_cast<const D3D11BlendState*>(renderItem.pBlendState);
		FLOAT blendFactor[4] = {
			renderItem.blendFactor.x,
			renderItem.blendFactor.y,
			renderItem.blendFactor.z,
			renderItem.blendFactor.w,
		};

		pContext->OMSetBlendState(bs->Get(), blendFactor, 0xffffffff); // sampleMask : masking MSAA's sample?
	}

	pContext->DrawIndexed(mesh->GetIndexCount(), 0, 0);
}

void RRenderer::DrawTessellatedQuad(const RenderItem& renderItem)
{
	const RMeshGeometry* mesh = renderItem.pMeshGeometry;
	auto* pContext = m_deviceResources->GetD3DDeviceContext();


	// Geometry Pipeline
	{
		pContext->IASetInputLayout(Graphics::BASIC_IL->Get());
		pContext->IASetPrimitiveTopology(GetD3D11TopologyType(mesh->GetTopologyType()));

		ID3D11Buffer* pVB[1] = { mesh->GetVertexBuffer() };
		UINT mVS = mesh->GetVertexStride();
		UINT mVO = mesh->GetVertexOffset();
		pContext->IASetVertexBuffers(0, 1, pVB, &mVS, &mVO);
		pContext->IASetIndexBuffer(mesh->GetIndexBuffer(), mesh->GetIndexFormat(), 0);


		pContext->VSSetShader(Graphics::BASIC_VS->Get(), 0, 0);
		pContext->DSSetShader(Graphics::TESSELATED_QUAD_DS->Get(), 0, 0);
		pContext->HSSetShader(Graphics::TESSELLATED_QUAD_HS->Get(), 0, 0);


		m_resourceManager->UpdateConstantBuffer(sizeof(RenderParam), &renderItem.meshParam, m_meshCB.Get());
		ID3D11Buffer* cbs[2] = { m_globalCB.Get(), m_meshCB.Get() };
		pContext->VSSetConstantBuffers(0, 2, cbs);
		pContext->HSSetConstantBuffers(0, 2, cbs);
		pContext->DSSetConstantBuffers(0, 2, cbs);
	}


	SetPipelineState(renderItem);
	m_resourceManager->UpdateConstantBuffer(sizeof(RenderParam), &renderItem.materialParam, m_materialCB.Get());

	if (renderItem.bIsTransparent)
	{
		FLOAT blendFactor[4] = {
			renderItem.blendFactor.x,
			renderItem.blendFactor.y,
			renderItem.blendFactor.z,
			renderItem.blendFactor.w,
		};

		pContext->OMSetBlendState(renderItem.pBlendState->Get(), blendFactor, 0);
	}


	pContext->DrawIndexed(mesh->GetIndexCount(), 0, 0);

	pContext->DSSetShader(nullptr, 0, 0);
	pContext->HSSetShader(nullptr, 0, 0);
}

