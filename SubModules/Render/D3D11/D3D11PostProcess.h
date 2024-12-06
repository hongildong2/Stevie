#pragma once
#include "pch.h"
#include "D3D11Texture.h"

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

class D3D11PostProcess final
{
public:
	D3D11PostProcess() = default;
	~D3D11PostProcess() = default;

	D3D11PostProcess(const D3D11PostProcess& other) = delete;
	D3D11PostProcess& operator=(const D3D11PostProcess& other) = delete;

	void Initialize(D3D11Renderer* pRenderer);

	void BeginPostProcess(std::unique_ptr<RTexture>& sourceRenderTarget);
	void Process();
	void EndPostProcess(ID3D11RenderTargetView* pRTVToDraw);

private:
	void ProcessFog();
	void ProcessBloom();
	void DrawScreenQuad();
private:
	enum
	{
		LEVEL = 4,
	};
	D3D11Renderer* m_pRenderer;

	std::unique_ptr<RTexture> m_renderTargetToProcess;
	std::unique_ptr<RTexture> m_renderTargetProcessed;

	std::unique_ptr<RMeshGeometry> m_screenQuad;

	std::vector<std::unique_ptr<RTexture>> m_blurTextures;

	PostProcessConstant m_postProcessConstant;
	ComPtr<ID3D11Buffer> m_postProcessCB;

private:
	D3D11VertexShader* pQuadVS;
	D3D11InputLayout* pSamplingIL;
	D3D11PixelShader* pFogPS;
	D3D11PixelShader* pFilterCombinePS;
	D3D11RasterizerState* pBasicRS;

	D3D11ComputeShader* pUpBlurCS;
	D3D11ComputeShader* pDownBlurCS;

	D3D11SamplerState* pLinearClampSS;
};

