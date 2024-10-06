#include "pch.h"
#include "D3D11Renderer.h"
#include "D3DUtil.h"
#include "D3D11MeshGeometry.h"
#include "Core/Components/MeshComponent.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;


D3D11Renderer::D3D11Renderer()
	: m_deviceResources()
{
	m_deviceResources = std::make_unique<D3D11DeviceResources>();
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
	context->ClearRenderTargetView(backBufferRTV, Colors::Black);
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


RShader* D3D11Renderer::CreateShader(const EShaderType type, const WCHAR* name)
{
	return nullptr;
}

RMeshGeometry* D3D11Renderer::CreateBasicMeshGeometry(const EBasicGeometry type)
{
	return nullptr;
}

RMeshGeometry* D3D11Renderer::CreateMeshGeometry(const void* pInVertexList, const UINT vertexSize, const UINT vertexCount, const void* pInIndexList, const UINT indexSize, const UINT indexCount)
{
	return nullptr;
}

RTexture2D* D3D11Renderer::CreateTextureFromFile(const WCHAR* wchFileName)
{
	return nullptr;
}

void D3D11Renderer::Render(const MeshComponent* pInMeshComponent)
{
	const D3D11MeshGeometry* mesh = static_cast<const D3D11MeshGeometry*>(pInMeshComponent->GetMeshGeometry());
	const RMaterial* mat = pInMeshComponent->GetMaterial();
	auto* pContext = m_deviceResources->GetD3DDeviceContext();

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

BOOL D3D11Renderer::ComputeOcean(const OceanMeshComponent* pInOcean)
{
	return 0;
}

BOOL D3D11Renderer::ComputeCloud(const CloudMeshComponent* pInCloud)
{
	return 0;
}

RTextureVolume* D3D11Renderer::CreateTextureVolume(const UINT width, const UINT height, const UINT depth, const DXGI_FORMAT format)
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
