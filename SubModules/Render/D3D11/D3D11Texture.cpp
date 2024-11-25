#include "pch.h"
#include "D3D11Renderer.h"
#include "D3D11Texture.h"
#include "D3DUtil.h"
#include "D3D11DeviceResources.h"

D3D11Texture2D::D3D11Texture2D()
	: D3D11Texture(ETextureType::TEXTURE_2D)
{
}

D3D11Texture3D::D3D11Texture3D()
	:D3D11Texture(ETextureType::TEXTURE_3D)
{
}


D3D11TextureCube::D3D11TextureCube()
	: D3D11Texture(ETextureType::TEXTURE_CUBE)
{
}

D3D11Texture::D3D11Texture(ETextureType type)
	: RTexture(type)
{
}

void D3D11Texture::Initialize(const UINT width, const UINT height, const UINT depth, const DXGI_FORMAT format, const BOOL bReadOnly)
{
	m_width = width;
	m_height = height;
	m_depth = depth;
	m_format = format;
	m_bIsReadOnly = bReadOnly;
	m_bInitialized = TRUE;
}

D3D11TextureRender::D3D11TextureRender(const DXGI_FORMAT format)
	: D3D11Texture(ETextureType::TEXTURE_2D)
{
	m_format = format;
}

D3D11StructuredBuffer::D3D11StructuredBuffer()
	: D3D11Texture(ETextureType::STRUCTURED_BUFFER)
{
	m_width = 0;
	m_height = 0;
	m_depth = 0;
	m_format = DXGI_FORMAT_UNKNOWN;
}

void D3D11StructuredBuffer::Initialize(const UINT totalSizeInByte, const UINT elementSizeInByte, const UINT count)
{
	m_bIsReadOnly = true;
	m_totalSizeInByte = totalSizeInByte;
	m_elementSizeInByte = elementSizeInByte;
	m_elementCount = count;
}


/* RDepthTexture */
RDepthTexture::RDepthTexture(IRenderer* pRenderer)
	: m_pRenderer(static_cast<D3D11Renderer*>(pRenderer))
	, m_format(DXGI_FORMAT_R32_TYPELESS)
	, m_type(ETextureType::TEXTURE_2D)
	, m_width(0)
	, m_height(0)
{}

void RDepthTexture::SetSize(const UINT width, const UINT height)
{
	m_width = width;
	m_height = height;
}

void RDepthTexture::Initialize()
{
	MY_ASSERT(m_width != 0 && m_height != 0);

	if (m_tex != nullptr)
	{
		Reset();
	}

	auto* pDevice = m_pRenderer->GetDeviceResources()->GetD3DDevice();
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Format = DXGI_FORMAT_R32_TYPELESS;
	desc.Width = m_width;
	desc.Height = m_width;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = 0;

	ThrowIfFailed(pDevice->CreateTexture2D(&desc, nullptr, m_tex.ReleaseAndGetAddressOf()));

	CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R32_FLOAT);

	ThrowIfFailed(pDevice->CreateShaderResourceView(
		m_tex.Get(),
		&shaderResourceViewDesc,
		m_SRV.ReleaseAndGetAddressOf()
	));

	CD3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc(D3D11_DSV_DIMENSION_TEXTURE2D, DXGI_FORMAT_D32_FLOAT);
	ThrowIfFailed(pDevice->CreateDepthStencilView(
		m_tex.Get(),
		&dsvDesc,
		m_DSV.ReleaseAndGetAddressOf()
	));
}

void RDepthTexture::Reset()
{
	m_tex.Reset();
	m_SRV.Reset();
	m_DSV.Reset();
}
