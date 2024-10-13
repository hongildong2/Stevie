#include "pch.h"
#include "D3D11Renderer.h"
#include "D3D11Texture.h"
#include "D3DUtil.h"
#include "D3D11DeviceResources.h"

void D3D11Texture2D::Initialize(const D3D11Renderer* pRenderer, const UINT width, const UINT height, const DXGI_FORMAT format, const BOOL bIsDynamic)
{
	auto* pDevice = pRenderer->GetDeviceResources()->GetD3DDevice();
	m_width = width;
	m_height = height;
	m_format = format;
	m_bIsDynamic = bIsDynamic;

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

	if (bIsDynamic)
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

	if (bIsDynamic)
	{
		CD3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc(D3D11_UAV_DIMENSION_TEXTURE2D, m_format);
		DX::ThrowIfFailed(pDevice->CreateUnorderedAccessView(m_resource.Get(), NULL, m_UAV.ReleaseAndGetAddressOf()));
	}
}

void D3D11Texture2D::InitializeFromFile(const D3D11Renderer* pRenderer, const WCHAR* pPath)
{
	auto* pDevice = pRenderer->GetDeviceResources()->GetD3DDevice();
	m_bIsDynamic = FALSE;

	DX::ThrowIfFailed(DirectX::CreateWICTextureFromFileEx(pDevice, pPath, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_FLAG(false), DirectX::WIC_LOADER_DEFAULT, nullptr, m_SRV.GetAddressOf()));
}

void D3D11Texture3D::Initialize(const D3D11Renderer* pRenderer, const UINT width, const UINT height, const UINT depth, const DXGI_FORMAT format, const BOOL bIsDynamic)
{

	auto* pDevice = pRenderer->GetDeviceResources()->GetD3DDevice();
	m_width = width;
	m_height = height;
	m_depth = depth;
	m_format = format;
	m_bIsDynamic = bIsDynamic;

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

	if (bIsDynamic)
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

	if (bIsDynamic)
	{
		CD3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc(D3D11_UAV_DIMENSION_TEXTURE3D, m_format);
		DX::ThrowIfFailed(pDevice->CreateUnorderedAccessView(m_resource.Get(), NULL, m_UAV.ReleaseAndGetAddressOf()));
	}
}
