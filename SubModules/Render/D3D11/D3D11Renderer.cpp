#include "pch.h"

#include "D3D11Texture.h"
#include "D3D11Renderer.h"
#include "D3DUtil.h"
#include "D3D11MeshGeometry.h"
#include "Core/Components/MeshComponent.h"
#include "SubModules/Render/Scene/Camera.h"
#include "../RTexture.h"
#include "../RMaterial.h"
#include "../GraphicsCommon1.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector4;


D3D11Renderer::D3D11Renderer()
	: m_deviceResources()
	, m_resourceManager()
	, m_camera(nullptr)
	, m_skybox(nullptr)
{
	m_deviceResources = std::make_unique<D3D11DeviceResources>();
	m_resourceManager = std::make_unique<D3D11ResourceManager>();
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

	m_resourceManager->CreateConstantBuffer(sizeof(GlobalConstant), nullptr, m_globalCB.GetAddressOf());
	m_resourceManager->CreateConstantBuffer(sizeof(MeshConstant), nullptr, m_meshCB.GetAddressOf());
	m_resourceManager->CreateConstantBuffer(sizeof(MaterialConstant), nullptr, m_materialCB.GetAddressOf());



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
		// TODO :: Create Float RTV

		// context->ClearRenderTargetView(m_floatRTV.Get(), Colors::Black); // HDR Pipeline, using float RTV
		context->ClearRenderTargetView(backBufferRTV, Colors::Cyan);
		context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


		ID3D11RenderTargetView* rtvs[1] =
		{
			backBufferRTV,
		};
		context->OMSetRenderTargets(1, rtvs, depthStencil);

		// Set the viewport.
		auto const viewport = m_deviceResources->GetScreenViewport();
		context->RSSetViewports(1, &viewport);
		m_deviceResources->PIXEndEvent();
	}

	SetGlobalConstant();

	// Render Skybox
	{
		m_deviceResources->PIXBeginEvent(L"Sky Box");

		m_deviceResources->PIXEndEvent();
	}

}

void D3D11Renderer::EndRender()
{
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

	return TRUE;
}

BOOL D3D11Renderer::UpdateWindowSize(DWORD dwBackBufferWidth, DWORD dwBackBufferHeight)
{
	if (!m_deviceResources)
	{
		return FALSE;
	}

	return m_deviceResources->WindowSizeChanged(dwBackBufferWidth, dwBackBufferHeight);
}


RMeshGeometry* D3D11Renderer::CreateMeshGeometry(const void* pInVertexList, const UINT vertexSize, const UINT vertexCount, const void* pInIndexList, const UINT indexSize, const UINT indexCount)
{
	return static_cast<RMeshGeometry*>(m_resourceManager->CreateMeshGeometry(pInVertexList, vertexSize, vertexCount, pInIndexList, indexSize, indexCount));
}

RTexture2D* D3D11Renderer::CreateTextureFromFile(const WCHAR* wchFileName)
{
	return static_cast<RTexture2D*>(m_resourceManager->CreateTextureFromFile(wchFileName));
}

RTextureCube* D3D11Renderer::CreateTextureCubeFromFile(const WCHAR* wchFileName)
{
	return static_cast<RTextureCube*>(m_resourceManager->CreateTextureCubeFromFile(wchFileName));
}

void D3D11Renderer::Render(const MeshComponent* pInMeshComponent, Matrix worldRow)
{
	const D3D11MeshGeometry* mesh = static_cast<const D3D11MeshGeometry*>(pInMeshComponent->GetMeshGeometry());

	const RMaterial* mat = pInMeshComponent->GetMaterial();
	auto* pContext = m_deviceResources->GetD3DDeviceContext();

	D3D11InputLayout* basicIL = static_cast<D3D11InputLayout*>(Graphics::BASIC_IL);
	pContext->IASetInputLayout(basicIL->Get());

	SetMeshConstant(pInMeshComponent, worldRow);
	SetMaterialConstant(mat);

	ID3D11Buffer* cbs[3] = { m_globalCB.Get(), m_meshCB.Get(), m_materialCB.Get() };

	D3D11VertexShader* basicVS = static_cast<D3D11VertexShader*>(Graphics::BASIC_VS);
	pContext->VSSetShader(basicVS->Get(), nullptr, 0);
	pContext->VSSetConstantBuffers(0, 3, cbs);

	// No further resource for demoPS
	const D3D11PixelShader* demoPS = static_cast<const D3D11PixelShader*>(mat->GetShader());
	pContext->PSSetShader(demoPS->Get(), nullptr, 0);

	D3D11RasterizerState* basicRS = static_cast<D3D11RasterizerState*>(Graphics::SOLID_CW_RS);
	pContext->RSSetState(basicRS->Get());

	mesh->Draw();


}

void D3D11Renderer::Compute(const RTexture** pResults, const UINT resultsCount, const RTexture** pResources, const UINT resourcesCount, const RSamplerState** pSamplerStates, const UINT samplerStatesCount, const void** alignedConstants, const UINT** constantSizes, const UINT constantsCount, const UINT batchX, const UINT batchY, const UINT batchZ)
{
}


RTexture3D* D3D11Renderer::CreateTexture3D(const UINT width, const UINT height, const UINT depth, const DXGI_FORMAT format)
{
	return nullptr;
}

RTexture2D* D3D11Renderer::CreateDynamicTexture(const UINT width, const UINT height, const DXGI_FORMAT format)
{
	return nullptr;
}


RMeshGeometry* D3D11Renderer::CreateQuadPatches(const UINT patchCount)
{
	return nullptr;
}

void D3D11Renderer::SetCamera(const Camera* pCamera)
{
	// camera not null
	m_camera = pCamera;
}

void D3D11Renderer::SetSkybox(const Skybox* pSkybox)
{
	m_skybox = pSkybox;
}

void D3D11Renderer::SetGlobalConstant()
{
	GlobalConstant globalConstant;
	ZeroMemory(&globalConstant, sizeof(GlobalConstant));


	Matrix viewRow = m_camera->GetViewRowMat();
	Matrix projRow = m_camera->GetProjRowMat();
	Matrix viewProjRow = viewRow * projRow;

	globalConstant.view = viewRow.Transpose();
	globalConstant.proj = projRow.Transpose();
	globalConstant.viewProj = viewProjRow.Transpose();

	globalConstant.invView = globalConstant.view.Invert();
	globalConstant.invProj = globalConstant.proj.Invert();
	globalConstant.invViewProj = globalConstant.viewProj.Invert();

	globalConstant.eyeWorld = m_camera->GetEyePos();
	globalConstant.globalTime = 0.f;

	globalConstant.eyeDir = m_camera->GetEyeDir();
	globalConstant.globalLightsCount = 0;

	globalConstant.nearZ = m_camera->GetNearZ();
	globalConstant.farZ = m_camera->GetFarZ();

	m_resourceManager->UpdateConstantBuffer(sizeof(GlobalConstant), &globalConstant, m_globalCB.Get());
}

void D3D11Renderer::SetMeshConstant(const MeshComponent* pMeshComponent, Matrix worldRow)
{
	MeshConstant meshCB;
	ZeroMemory(&meshCB, sizeof(MeshConstant));
	Matrix world = worldRow;
	Matrix worldInverse = world.Invert();
	Matrix worldIT = worldInverse.Transpose();

	meshCB.world = world.Transpose();
	meshCB.worldInv = worldInverse.Transpose();
	meshCB.worldIT = worldIT.Transpose();
	m_resourceManager->UpdateConstantBuffer(sizeof(MeshConstant), &meshCB, m_meshCB.Get());
}

void D3D11Renderer::SetMaterialConstant(const RMaterial* pMaterial)
{
	MaterialConstant materialConstant;
	ZeroMemory(&materialConstant, sizeof(MaterialConstant));
	// TODO :: Set by Material
}
