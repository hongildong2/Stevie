#include "pch.h"

void RTexture::Initialize()
{
	MY_ASSERT(m_SRV != nullptr);
	MY_ASSERT(m_width != 0);
	MY_ASSERT(m_height != 0);
	MY_ASSERT(m_depth != 0);
	MY_ASSERT(m_count != 0);

	MY_ASSERT(m_type != ETextureType::TEXTURE_2D_RENDER || m_RTV != nullptr);
	MY_ASSERT(m_type != ETextureType::TEXTURE_2D_DEPTH || m_DSV != nullptr);
	MY_ASSERT(m_bIsDynamic != FALSE || m_UAV == nullptr);

	m_bInitialized = TRUE;
}

