#pragma once
#include <vector>
#include "DeviceResources.h"
#include "RenderTexture.h"
#include "MeshPart.h"

struct PostProcessConstant
{
	float dx;
	float dy;
	float dummy1;
	float strength;
	float exposure;
	float gamma;
	float blur;
	float dummy;
};

constexpr PostProcessConstant DEFAULT_POST_PROCESS_PARAM =
{
	0.f,
	0.f,
	0.f,
	0.9f,
	1.1f,
	2.2f,
	1.f,
	0.f,
};

class PostProcess
{
public:
	PostProcess(const RECT size, DXGI_FORMAT pipelineFormat);
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
	// Bloom
	RECT m_originalSize;
	std::vector<std::unique_ptr<RenderTexture>> m_bloomTextures;
	PostProcessConstant m_postProcessConstant;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_postProcessCB;
};

