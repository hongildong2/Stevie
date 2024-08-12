#pragma once
#include "pch.h"

#include "DeviceResources.h"
#include "MeshPart.h"
#include "RenderTexture.h"
#include "AObject.h"
#include "IGUIComponent.h"

struct PostProcessConstant
{
	float dx;
	float dy;
	float dummy1;
	float strength;
	float exposure;
	float gamma;
	float blur;
	float fogStrength;
};

constexpr PostProcessConstant DEFAULT_POST_PROCESS_PARAM =
{
	0.f,
	0.f,
	0.f,
	0.8f,
	0.3f,
	2.4f,
	1.f,
	1.f,
};

class MyPostProcess final : public AObject, public IGUIComponent
{
public:
	MyPostProcess(const RECT size, DXGI_FORMAT pipelineFormat);
	~MyPostProcess() = default;

	MyPostProcess(const MyPostProcess& other) = delete;
	MyPostProcess& operator=(const MyPostProcess& other) = delete;

	void Initialize(ID3D11Device1* device);
	void ProcessBloom(ID3D11DeviceContext1* context);
	void ProcessFog(ID3D11DeviceContext1* pContext, ID3D11ShaderResourceView* depthOnlySRV);

	void Draw(ID3D11DeviceContext1* context, ID3D11RenderTargetView* rtvToDraw);
	void UpdateConstant(PostProcessConstant constant);

	void FillTextureToProcess(ID3D11DeviceContext1* pContext, ID3D11Texture2D* pRenderedBuffer);

	AObject* GetThis() override;

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

