#include "pch.h"

#include "D3D11Texture.h"
#include "D3D11Renderer.h"
#include "D3DUtil.h"
#include "D3D11MeshGeometry.h"
#include "../RTexture.h"
#include "Core/Components/MeshComponent.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;


D3D11Renderer::D3D11Renderer()
	: m_deviceResources()
	, m_resourceManager()
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

	// Init Shader, States, Managers..

	return TRUE;
}

void D3D11Renderer::BeginRender()
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

void D3D11Renderer::Render(const MeshComponent* pInMeshComponent)
{
	D3D11MeshGeometry* mesh = static_cast<D3D11MeshGeometry*>(pInMeshComponent->GetMeshGeometry());

	const RMaterial* mat = pInMeshComponent->GetMaterial();
	auto* pContext = m_deviceResources->GetD3DDeviceContext();

	// mesh->Draw();

	// single pass rendering
	// 
		// RSstate, basic
		// Depth/Stencil, basic


		// set BlendState from material
		// set Pixel Shader from material
		// Get Textures from Material, Set as Resource
		// Set SamplerState of material
		// 
		// mesh->Draw
}


// TODO :: 
void D3D11Renderer::RenderOcean(const OceanMeshComponent* pInOcean)
{
}

void D3D11Renderer::RenderCloud(const CloudMeshComponent* pInRender)
{
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

