#include "pch.h"
#include "SubModules/Render/RResourceManager.h"
#include "SubModules/Render/RPostProcess.h"
#include "SubModules/Render/RBuffer.h"
#include "D3D11Resources.h"
#include "D3D11DeviceResources.h"
#include "D3DUtil.h"
#include "D3D11MeshGeometry.h"
#include "Core/Components/MeshComponent.h"
#include "Core/Skybox.h"
#include "Core/Camera.h"
#include "Core/Light.h"

RRenderer::RRenderer()
	: m_resourceManager()
	, m_postProcess()
	, m_renderItemIndex(0)
	, m_renderItems{}
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
	, m_shadowingLightsIndices{}
	, m_shadowingLightsIndex(0)
{
	m_deviceResources = std::make_unique<D3D11DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_R32_TYPELESS);
	m_resourceManager = std::make_unique<RResourceManager>();
	m_postProcess = std::make_unique<RPostProcess>();
	m_globalCB = std::make_unique<RBuffer>();
	m_sunLightCB = std::make_unique<RBuffer>();
	m_meshCB = std::make_unique<RBuffer>();
	m_materialCB = std::make_unique<RBuffer>();
	m_computeCB = std::make_unique<RBuffer>();
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

	m_resourceManager->CreateConstantBuffer(sizeof(RGlobalConstant), nullptr, m_globalCB.get());
	m_resourceManager->CreateConstantBuffer(sizeof(RLightConstant), nullptr, m_sunLightCB.get());
	m_resourceManager->CreateConstantBuffer(sizeof(RenderParam), nullptr, m_meshCB.get());
	m_resourceManager->CreateConstantBuffer(sizeof(RenderParam), nullptr, m_materialCB.get());
	m_resourceManager->CreateConstantBuffer(sizeof(RenderParam), nullptr, m_computeCB.get());

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


void RRenderer::EndRender()
{
	// Post Process
	auto* backBufferRTV = m_deviceResources->GetRenderTargetView();
	m_postProcess->BeginPostProcess(m_HDRRenderTarget);
	m_postProcess->Process();
	m_postProcess->EndPostProcess();

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
		m_resourceManager->UpdateConstantBuffer(sizeof(RenderParam), pAlignedComputeParam, m_computeCB.get());
		pContext->CSSetConstantBuffers(0, 1, m_computeCB->GetAddressOf());
	}

	pContext->Dispatch(batchX, batchY, batchZ);

	ID3D11ShaderResourceView* releaseSRV[MAX_COMPUTE_RESOURCE_COUNT] = { NULL, };
	ID3D11UnorderedAccessView* releaseUAV[MAX_COMPUTE_RESOURCE_COUNT] = { NULL, };
	// release
	pContext->CSSetShaderResources(0, MAX_COMPUTE_RESOURCE_COUNT, releaseSRV);
	pContext->CSSetUnorderedAccessViews(0, MAX_COMPUTE_RESOURCE_COUNT, releaseUAV, NULL);
	m_deviceResources->PIXEndEvent();
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

	m_resourceManager->UpdateConstantBuffer(sizeof(RGlobalConstant), &globalConstant, m_globalCB.get());

	// Sun Light
	RLightConstant lightData = m_sunLight->GetLightConstant();
	m_resourceManager->UpdateConstantBuffer(sizeof(RLightConstant), &lightData, m_sunLightCB.get());
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
		ID3D11Buffer* cbs[4] = { m_globalCB->Get(), m_meshCB->Get(), m_materialCB->Get(), m_sunLightCB->Get() };
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


		ID3D11Buffer* cbs[1] = { m_globalCB->Get() };
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


		m_resourceManager->UpdateConstantBuffer(sizeof(RenderParam), &renderItem.meshParam, m_meshCB.get());
		ID3D11Buffer* cbs[2] = { m_globalCB->Get(), m_meshCB->Get() };
		pContext->VSSetConstantBuffers(0, 2, cbs);
	}


	SetPipelineState(renderItem);
	m_resourceManager->UpdateConstantBuffer(sizeof(RenderParam), &renderItem.materialParam, m_materialCB.get());

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


		m_resourceManager->UpdateConstantBuffer(sizeof(RenderParam), &renderItem.meshParam, m_meshCB.get());
		ID3D11Buffer* cbs[2] = { m_globalCB->Get(), m_meshCB->Get() };
		pContext->VSSetConstantBuffers(0, 2, cbs);
		pContext->HSSetConstantBuffers(0, 2, cbs);
		pContext->DSSetConstantBuffers(0, 2, cbs);
	}


	SetPipelineState(renderItem);
	m_resourceManager->UpdateConstantBuffer(sizeof(RenderParam), &renderItem.materialParam, m_materialCB.get());

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

