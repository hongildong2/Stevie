#pragma once
#include "pch.h"
#include "DeviceResources.h"
#include <vector>
#include "RenderTexture.h"

struct PostProcessConstant
{
	float dx;
	float dy;
	float dummy1;
	float strength = 0.2f;
	float exposure = 1.f; // option1 in c++
	float gamma = 2.2f; // option2 in c++
	float blur = 1.f; // option3 in c++
	float dummy;
};

class PostProcess
{
public:
	void Initialize(ID3D11Device1* device, const RECT size);
	void ProcessBloom(ID3D11DeviceContext1* context);
	void ProcessFog(ID3D11DeviceContext1* context, ID3D11ShaderResourceView* depthOnlySRV);
	void Draw(ID3D11DeviceContext1* context);
	void UpdateConstant(PostProcessConstant constant);

	// TODO : Release
	inline ID3D11RenderTargetView* GetRenderTargetView() const
	{
		return m_textures[0]->GetRenderTargetView();
	}

	inline ID3D11Texture2D* GetFirstTexture() const
	{
		return m_textures[0]->GetRenderTarget();
	}

	inline ID3D11ShaderResourceView* GetFirstSRV() const
	{
		return m_textures[0]->GetShaderResourceView();
	}

	inline PostProcessConstant GetConstant() const
	{
		return m_postProcessConstant;
	}


	enum { LEVEL = 4 };
private:
	RECT m_originalSize;
	PostProcessConstant m_postProcessConstant;
	std::unique_ptr<Model> m_screenQuad;

	// these resources' index indicates mips level 0, 1, 2 itself
	std::vector<std::unique_ptr<RenderTexture>> m_textures;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_postProcessCB;
};

