#include "pch.h"
#include "D3D11Renderer.h"
#include "D3D11Texture.h"
#include "D3DUtil.h"
#include "D3D11DeviceResources.h"

D3D11Texture2D::D3D11Texture2D()
	: D3D11Texture(ETextureType::TEXTURE_2D)
{
}

void D3D11Texture2D::Initialize(const D3D11Renderer* pRenderer, const UINT width, const UINT height, const DXGI_FORMAT format, const BOOL bIsReadOnly)
{
	auto* pDevice = pRenderer->GetDeviceResources()->GetD3DDevice();
	m_width = width;
	m_height = height;
	m_format = format;
	m_bIsReadOnly = bIsReadOnly;

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Format = m_format;
	desc.Width = m_width;
	desc.Height = m_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = 0;

	if (!bIsReadOnly)
	{
		desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
	}

	DX::ThrowIfFailed(pDevice->CreateTexture2D(&desc, nullptr, m_resource.GetAddressOf()));

	CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(D3D11_SRV_DIMENSION_TEXTURE2D, m_format);

	DX::ThrowIfFailed(pDevice->CreateShaderResourceView(
		m_resource.Get(),
		&shaderResourceViewDesc,
		m_SRV.ReleaseAndGetAddressOf()
	));

	if (!bIsReadOnly)
	{
		CD3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc(D3D11_UAV_DIMENSION_TEXTURE2D, m_format);
		DX::ThrowIfFailed(pDevice->CreateUnorderedAccessView(m_resource.Get(), NULL, m_UAV.ReleaseAndGetAddressOf()));
	}
}

void D3D11Texture2D::InitializeFromFile(const D3D11Renderer* pRenderer, const WCHAR* pPath)
{
	auto* pDevice = pRenderer->GetDeviceResources()->GetD3DDevice();
	m_bIsReadOnly = TRUE;

	DX::ThrowIfFailed(CreateWICTextureFromFileEx(pDevice, pPath, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_FLAG(false), DirectX::WIC_LOADER_DEFAULT, nullptr, m_SRV.GetAddressOf()));
}

void D3D11Texture2D::InitializeFromDDSFile(const D3D11Renderer* pRenderer, const WCHAR* path)
{
	auto* pDevice = pRenderer->GetDeviceResources()->GetD3DDevice();
	m_bIsReadOnly = TRUE;

	DX::ThrowIfFailed(CreateDDSTextureFromFileEx(pDevice, path, 0, D3D11_USAGE_DEFAULT, D3D10_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_FLAG(false), DDS_LOADER_DEFAULT, nullptr, m_SRV.GetAddressOf(), nullptr));
}

D3D11Texture3D::D3D11Texture3D()
	:D3D11Texture(ETextureType::TEXTURE_3D)
{
}

void D3D11Texture3D::Initialize(const D3D11Renderer* pRenderer, const UINT width, const UINT height, const UINT depth, const DXGI_FORMAT format, const BOOL bIsReadOnly)
{

	auto* pDevice = pRenderer->GetDeviceResources()->GetD3DDevice();
	m_width = width;
	m_height = height;
	m_depth = depth;
	m_format = format;
	m_bIsReadOnly = bIsReadOnly;

	D3D11_TEXTURE3D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Format = m_format;
	desc.Width = m_width;
	desc.Height = m_height;
	desc.Depth = m_depth;
	desc.MipLevels = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = 0;

	if (!bIsReadOnly)
	{
		desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
	}

	DX::ThrowIfFailed(pDevice->CreateTexture3D(&desc, nullptr, m_resource.GetAddressOf()));

	CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(D3D11_SRV_DIMENSION_TEXTURE3D, m_format);

	DX::ThrowIfFailed(pDevice->CreateShaderResourceView(
		m_resource.Get(),
		&shaderResourceViewDesc,
		m_SRV.ReleaseAndGetAddressOf()
	));

	if (!bIsReadOnly)
	{
		CD3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc(D3D11_UAV_DIMENSION_TEXTURE3D, m_format);
		DX::ThrowIfFailed(pDevice->CreateUnorderedAccessView(m_resource.Get(), NULL, m_UAV.ReleaseAndGetAddressOf()));
	}
}

D3D11TextureCube::D3D11TextureCube()
	: D3D11Texture(ETextureType::TEXTURE_CUBE)
{
}

void D3D11TextureCube::Initialize(const D3D11Renderer* pRenderer, const WCHAR* path)
{
	auto* pDevice = pRenderer->GetDeviceResources()->GetD3DDevice();
	m_bIsReadOnly = true;
	DX::ThrowIfFailed(CreateDDSTextureFromFileEx(pDevice, path, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D10_RESOURCE_MISC_FLAG(false), DDS_LOADER_DEFAULT, nullptr, m_SRV.GetAddressOf(), nullptr));
}

D3D11Texture::D3D11Texture(ETextureType type)
	: RTexture(type)
{
}

D3D11TextureDepth::D3D11TextureDepth()
	: D3D11Texture(ETextureType::TEXTURE_2D)
{
}

void D3D11TextureDepth::Initialize(const D3D11Renderer* pRenderer, const UINT width, const UINT height)
{
	auto* pDevice = pRenderer->GetDeviceResources()->GetD3DDevice();
	m_width = width;
	m_height = height;
	m_format = DXGI_FORMAT_R32_TYPELESS;
	m_bIsReadOnly = true;

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Format = m_format;
	desc.Width = m_width;
	desc.Height = m_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = 0;

	DX::ThrowIfFailed(pDevice->CreateTexture2D(&desc, nullptr, m_resource.GetAddressOf()));

	CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R32_FLOAT);

	DX::ThrowIfFailed(pDevice->CreateShaderResourceView(
		m_resource.Get(),
		&shaderResourceViewDesc,
		m_SRV.ReleaseAndGetAddressOf()
	));

	CD3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc(D3D11_DSV_DIMENSION_TEXTURE2D, DXGI_FORMAT_D32_FLOAT);
	DX::ThrowIfFailed(pDevice->CreateDepthStencilView(
		m_resource.Get()
		, &dsvDesc
		, m_DSV.ReleaseAndGetAddressOf()
	));

}

D3D11TextureRender::D3D11TextureRender(const DXGI_FORMAT format)
	: D3D11Texture(ETextureType::TEXTURE_2D)
{
	m_format = format;
}

void D3D11TextureRender::Initialize(const D3D11Renderer* pRenderer, const UINT width, const UINT height)
{
	MY_ASSERT(pRenderer != nullptr);
	auto* pDevice = pRenderer->GetDeviceResources()->GetD3DDevice();

	{
		UINT formatSupport = 0;
		if (FAILED(pDevice->CheckFormatSupport(m_format, &formatSupport)))
		{
			throw std::runtime_error("CheckFormatSupport");
		}

		UINT32 required = D3D11_FORMAT_SUPPORT_TEXTURE2D | D3D11_FORMAT_SUPPORT_RENDER_TARGET;
		if ((formatSupport & required) != required)
		{
			throw std::runtime_error("RenderTexture");
		}
	}

	m_width = width;
	m_height = height;

	if (m_width > UINT32_MAX || m_height > UINT32_MAX)
	{
		throw std::out_of_range("Invalid width/height");
	}

	m_resource.Reset();
	m_RTV.Reset();
	m_SRV.Reset();
	m_UAV.Reset();

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Format = m_format;
	desc.Width = m_width;
	desc.Height = m_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = 0;

	DX::ThrowIfFailed(pDevice->CreateTexture2D(
		&desc,
		nullptr,
		m_resource.GetAddressOf()
	));

	// Create RTV.
	CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2D, m_format);

	DX::ThrowIfFailed(pDevice->CreateRenderTargetView(
		m_resource.Get(),
		&renderTargetViewDesc,
		m_RTV.ReleaseAndGetAddressOf()
	));

	// Create SRV.
	CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(D3D11_SRV_DIMENSION_TEXTURE2D, m_format);

	DX::ThrowIfFailed(pDevice->CreateShaderResourceView(
		m_resource.Get(),
		&shaderResourceViewDesc,
		m_SRV.ReleaseAndGetAddressOf()
	));

	CD3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc(D3D11_UAV_DIMENSION_TEXTURE2D, m_format);
	DX::ThrowIfFailed(pDevice->CreateUnorderedAccessView(m_resource.Get(), NULL, m_UAV.ReleaseAndGetAddressOf()));
}
