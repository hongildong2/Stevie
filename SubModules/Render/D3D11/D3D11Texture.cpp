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

D3D11TextureDepth::D3D11TextureDepth()
	: D3D11Texture(ETextureType::TEXTURE_2D)
{
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
