#pragma once
#include "pch.h"


class RTexture2D;
class RShader;
class RSamplerState;
class RBlendState;
class IRenderResource;
class RPixelShader;

constexpr UINT MATERIAL_MAX_TEXTURE_SLOT = 10;
class RMaterial // PBR Material, Ocean Material, ...
{
public:
	inline const RPixelShader* GetShader() const
	{
		return m_pixelShader;
	}

	// StructuredBuffer?
	const UINT GetShaderResourceCount();
	const UINT GetConstantBufferCount();
	const UINT GetSamplerStateCount();
	const RTexture2D* GetTextureResources();
	const RSamplerState* GetSamplerStates();
	const RBlendState* GetBlendState();

protected:
	RTexture2D* m_textures[MATERIAL_MAX_TEXTURE_SLOT];
	// OM stage settings, blend state, depth ..., stencil ..
	// Sampler state???
	RPixelShader* m_pixelShader;
};

