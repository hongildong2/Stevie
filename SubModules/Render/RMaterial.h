#pragma once
#include "pch.h"


class RTexture2D;
class RShader;
class RSamplerState;
class RBlendState;
class RPixelShader;

constexpr UINT MATERIAL_MAX_TEXTURE_SLOT = 10;
class RMaterial
{
public:
	inline const RPixelShader* GetShader() const
	{
		return m_pixelShader;
	}
	virtual const RSamplerState* GetSamplerStates() const
	{
		return m_samplerState;
	}
	virtual const RBlendState* GetBlendState() const
	{
		return m_blendState;
	}

	virtual void Initialize() = 0; // Set Resources Here
	virtual const UINT GetShaderResourceCount() = 0;
	virtual const UINT GetConstantBufferCount() = 0;
	virtual const UINT GetSamplerStateCount() = 0;
	virtual RTexture2D* const* GetTextureResources() = 0;


protected:
	// Owning Resources
	RTexture2D* m_textures[MATERIAL_MAX_TEXTURE_SLOT];

	// State, from Common Resource
	RPixelShader* m_pixelShader;
	RSamplerState* m_samplerState;
	RBlendState* m_blendState;
};



class RDemoMaterial : public RMaterial
{
public:
	RDemoMaterial() = default;
	~RDemoMaterial() = default;

	void Initialize() override;
	const UINT GetShaderResourceCount() override
	{
		return 0;
	}
	const UINT GetConstantBufferCount() override
	{
		return 0;
	}
	const UINT GetSamplerStateCount() override
	{
		return 0;
	}

	RTexture2D* const* GetTextureResources() override
	{
		return nullptr;
	}



};