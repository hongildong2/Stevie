#include "pch.h"
#include "RenderTexture.h"
#include "Utility.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

RenderTexture::RenderTexture(DXGI_FORMAT format) :
	m_format(format),
	m_width(0),
	m_height(0)
{
}

void RenderTexture::SetDevice(ID3D11Device* device)
{
	if (device == m_device.Get())
		return;

	if (m_device)
	{
		ReleaseDevice();
	}

	{
		UINT formatSupport = 0;
		if (FAILED(device->CheckFormatSupport(m_format, &formatSupport)))
		{
			throw std::runtime_error("CheckFormatSupport");
		}

		UINT32 required = D3D11_FORMAT_SUPPORT_TEXTURE2D | D3D11_FORMAT_SUPPORT_RENDER_TARGET;
		if ((formatSupport & required) != required)
		{
			throw std::runtime_error("RenderTexture");
		}
	}

	m_device = device;
}


void RenderTexture::SizeResources(size_t width, size_t height)
{
	if (width == m_width && height == m_height)
		return;

	if (m_width > UINT32_MAX || m_height > UINT32_MAX)
	{
		throw std::out_of_range("Invalid width/height");
	}

	if (!m_device)
		return;

	m_width = m_height = 0;

	// Create a render target
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Format = m_format; // for HDR Pipeline
	desc.Width = static_cast<UINT>(width);
	desc.Height = static_cast<UINT>(height);
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = 0;

	DX::ThrowIfFailed(m_device->CreateTexture2D(
		&desc,
		nullptr,
		m_renderTarget.GetAddressOf()
	));

	// Create RTV.
	CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2D, m_format);

	DX::ThrowIfFailed(m_device->CreateRenderTargetView(
		m_renderTarget.Get(),
		&renderTargetViewDesc,
		m_renderTargetView.ReleaseAndGetAddressOf()
	));

	// Create SRV.
	CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(D3D11_SRV_DIMENSION_TEXTURE2D, m_format);

	DX::ThrowIfFailed(m_device->CreateShaderResourceView(
		m_renderTarget.Get(),
		&shaderResourceViewDesc,
		m_shaderResourceView.ReleaseAndGetAddressOf()
	));

	CD3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc(D3D11_UAV_DIMENSION_TEXTURE2D, m_format);
	DX::ThrowIfFailed(m_device->CreateUnorderedAccessView(m_renderTarget.Get(), NULL, m_unorderedAccessView.ReleaseAndGetAddressOf()));

	m_width = width;
	m_height = height;
}


void RenderTexture::ReleaseDevice()
{
	m_renderTargetView.Reset();
	m_shaderResourceView.Reset();
	m_renderTarget.Reset();

	m_device.Reset();

	m_width = m_height = 0;
}

void RenderTexture::SetWindow(const RECT& output)
{
	// Determine the render target size in pixels.
	auto width = size_t(std::max<LONG>(output.right - output.left, 1));
	auto height = size_t(std::max<LONG>(output.bottom - output.top, 1));

	SizeResources(width, height);
}