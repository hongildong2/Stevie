#pragma once
#include "pch.h"


class RTexture2D;
class RShader;

constexpr UINT MATERIAL_MAX_TEXTURE_SLOT = 10;
class Material // PBR Material, Ocean Material, ...
{
public:
	inline const RShader* GetShader() const
	{
		return m_pixelShader;
	}


protected:
	RTexture2D* m_textures[MATERIAL_MAX_TEXTURE_SLOT];
	// OM stage settings, blend state, depth ..., stencil ..
	// Sampler state???
	RShader* m_pixelShader;
};

