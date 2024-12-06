#pragma once
#include "pch.h"

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
	0.3f,
};

class RPostProcess final
{
public:
	RPostProcess();
	~RPostProcess() = default;

	RPostProcess(const RPostProcess& other) = delete;
	RPostProcess& operator=(const RPostProcess& other) = delete;

	void Initialize(RRenderer* pRenderer);

	void BeginPostProcess(std::unique_ptr<RTexture>& sourceRenderTarget);
	void Process();
	void EndPostProcess();

private:
	void ProcessFog();
	void ProcessBloom();
	void DrawScreenQuad();
private:
	enum
	{
		LEVEL = 4,
	};
	RRenderer* m_pRenderer;

	std::unique_ptr<RTexture> m_renderTargetToProcess;
	std::unique_ptr<RTexture> m_renderTargetProcessed;

	std::unique_ptr<RMeshGeometry> m_screenQuad;

	std::vector<std::unique_ptr<RTexture>> m_blurTextures;

	PostProcessConstant m_postProcessConstant;
	ComPtr<ID3D11Buffer> m_postProcessCB;
};

