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

constexpr PostProcessConstant DEFAULT_POST_PROCESS_PARAM =
{
	0.f,
	0.f,
	0.f,
	0.2f,
	1.f,
	2.2f,
	1.f,
	0.f,
};

class PostProcess
{
public:
	PostProcess() = delete;
	PostProcess(const RECT size);

	~PostProcess() = default;

	PostProcess(const PostProcess& other) = delete;
	PostProcess& operator=(const PostProcess& other) = delete;

	void Initialize(ID3D11Device1* device);
	void ProcessBloom(ID3D11DeviceContext1* context);
	void ProcessFog(ID3D11DeviceContext1* pContext, ID3D11ShaderResourceView* depthOnlySRV);

	void Draw(ID3D11DeviceContext1* context, ID3D11RenderTargetView* rtvToDraw);
	void UpdateConstant(PostProcessConstant constant);

	void FillTextureToProcess(ID3D11DeviceContext1* pContext, ID3D11Texture2D* pRenderedBuffer);

	inline PostProcessConstant GetConstant() const
	{
		return m_postProcessConstant;
	}


	enum { LEVEL = 4 };
private:
	std::unique_ptr<MeshPart> m_screenQuad;
	std::unique_ptr<RenderTexture> m_textureToProcess;
	std::unique_ptr<RenderTexture> m_textureProcessed;


	// TODO : list of post effects
	// Fog


	// Bloom
	RECT m_originalSize;
	std::vector<std::unique_ptr<RenderTexture>> m_bloomTextures;
	PostProcessConstant m_postProcessConstant;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_postProcessCB;
};

