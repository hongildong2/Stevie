#include "pch.h"
#include "VolumetricTexture.h"
#include "Utility.h"

VolumetricTexture::VolumetricTexture(const DXGI_FORMAT format, const size_t width, const size_t height, const size_t depth)
	: m_format(format)
	, m_width(width)
	, m_height(height)
	, m_depth(depth)
{
}

void VolumetricTexture::SetDevice(ID3D11Device* pDevice)
{
	if (m_device.Get() == pDevice)
	{
		return;
	}

	if (m_device)
	{
		ReleaseDevice();
	}

	{
		UINT formatSupport = 0;
		if (FAILED(pDevice->CheckFormatSupport(m_format, &formatSupport)))
		{
			throw std::runtime_error("CheckFormatSupport");
		}

		UINT32 required = D3D11_FORMAT_SUPPORT_TEXTURE3D | D3D11_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW;
		if ((formatSupport & required) != required)
		{
			throw std::runtime_error("VolumetricTexture");
		}
	}

	m_device = pDevice;
}

void VolumetricTexture::ReleaseDevice()
{
	m_volumetricTexture.Reset();
	m_srv.Reset();
	m_uav.Reset();

	m_device.Reset();

	m_width = m_height = m_depth = 0;
}

void VolumetricTexture::Initialize()
{
	assert(m_device != nullptr);

	D3D11_TEXTURE3D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.Format = m_format;
	desc.Width = static_cast<UINT>(m_width);
	desc.Height = static_cast<UINT>(m_height);
	desc.Depth = static_cast<UINT>(m_depth);
	desc.MipLevels = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = 0;

	DX::ThrowIfFailed(m_device->CreateTexture3D(&desc, nullptr, m_volumetricTexture.GetAddressOf()));

	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE3D, m_format);
	DX::ThrowIfFailed(m_device->CreateShaderResourceView(m_volumetricTexture.Get(), &srvDesc, m_srv.GetAddressOf()));

	CD3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc(D3D11_UAV_DIMENSION_TEXTURE3D, m_format);
	DX::ThrowIfFailed(m_device->CreateUnorderedAccessView(m_uav.Get(), NULL, m_uav.GetAddressOf()));
}