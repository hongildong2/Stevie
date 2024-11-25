#include "pch.h"
#include "RTexture.h"

RTexture::RTexture(ETextureType type)
	: m_width(0)
	, m_height(0)
	, m_depth(0)
	, m_format(DXGI_FORMAT_UNKNOWN)
	, m_bIsReadOnly(TRUE)
	, m_textureType(type)
{
}
