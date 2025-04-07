#include "pch.h"
#include "D3D11Texture.h"
#include "D3D11Resources.h"
#include "D3D11MeshGeometry.h"
#include "D3D11DeviceResources.h"
#include "Game/GeometryGenerator.h"
#include "../RPostProcess.h"
#include "../RResourceManager.h"
#include "../RenderConfig.h"
#include "../RMeshGeometry.h"

RPostProcess::RPostProcess()
	: m_pRenderer(nullptr)
	, m_postProcessConstant(DEFAULT_POST_PROCESS_PARAM)
{
}

// move to RPostProcess.cpp later
void RPostProcess::Initialize(RRenderer* pRenderer)
{
	MY_ASSERT(pRenderer != nullptr);

	if (m_pRenderer != nullptr)
	{
		m_renderTargetProcessed.reset();
		m_renderTargetToProcess.reset();
		m_blurTextures.clear();
	}

	m_pRenderer = pRenderer;
	auto* pResourceManager = pRenderer->GetResourceManager();


	UINT renderTargetWidth = m_pRenderer->GetBackBufferWidth();
	UINT renderTargetHeight = m_pRenderer->GetBackBufferHeight();

	m_renderTargetToProcess = std::unique_ptr<RTexture>(pResourceManager->CreateTextureRender(renderConfig::HDR_PIPELINE_FORMAT, renderTargetWidth, renderTargetHeight));
	m_renderTargetProcessed = std::unique_ptr<RTexture>(pResourceManager->CreateTextureRender(renderConfig::HDR_PIPELINE_FORMAT, renderTargetWidth, renderTargetHeight));

	UINT blurRTWidth = renderTargetWidth;
	UINT blurRTHeight = renderTargetHeight;
	for (UINT i = 0; i <= LEVEL; ++i)
	{
		m_blurTextures.emplace_back(std::unique_ptr<RTexture>(pResourceManager->CreateTextureRender(renderConfig::HDR_PIPELINE_FORMAT, blurRTWidth, blurRTHeight)));
		blurRTWidth /= 2;
		blurRTHeight /= 2;
	}

}

void RPostProcess::BeginPostProcess(std::unique_ptr<RTexture>& sourceRenderTarget)
{
	m_renderTargetToProcess.swap(sourceRenderTarget);

	// release DSV, RTV from context
	auto* pContext = m_pRenderer->GetDeviceResources()->GetD3DDeviceContext();
	pContext->OMSetRenderTargets(0, NULL, NULL);
}

void RPostProcess::Process()
{
	ProcessFog();

	// Expected to be last
	ProcessBloom();
}

void RPostProcess::EndPostProcess()
{
	const RTexture* rtv = m_pRenderer->GetRenderTexture();
	const RTexture* srvs[2] = { m_renderTargetProcessed.get(), m_renderTargetToProcess.get() };
	const RSamplerState* ss[1] = { Graphics::LINEAR_CLAMP_SS };

	m_pRenderer->DrawScreenQuad(Graphics::FILTER_COMBINE_PS, srvs, _countof(srvs), ss, _countof(ss), rtv, CAST_RENDER_PARAM_PTR(&m_postProcessConstant));
}

void RPostProcess::ProcessFog()
{
	const RTexture* srvs[2] = { m_renderTargetToProcess.get(), m_pRenderer->GetDepthTexture() };
	const RSamplerState* ss[1] = { Graphics::LINEAR_CLAMP_SS };

	m_pRenderer->DrawScreenQuad(Graphics::FOG_PS, srvs, _countof(srvs), ss, _countof(ss), m_renderTargetProcessed.get(), CAST_RENDER_PARAM_PTR(&m_postProcessConstant));
	m_renderTargetProcessed.swap(m_renderTargetToProcess);
}

void RPostProcess::ProcessBloom()
{
	UINT renderTargetWidth = m_pRenderer->GetBackBufferWidth();
	UINT renderTargetHeight = m_pRenderer->GetBackBufferHeight();

	unsigned int group_x = static_cast<unsigned int>(ceil(renderTargetWidth / 32.f));
	unsigned int group_y = static_cast<unsigned int>(ceil(renderTargetHeight / 32.f));

	const RSamplerState* samplers[1] = { Graphics::LINEAR_CLAMP_SS };
	// swap m_bloomTextures[0] with SourceTexture
	m_blurTextures[0].swap(m_renderTargetToProcess);

	for (size_t i = 0; i < LEVEL; ++i)
	{
		const RTexture* from[1] = { m_blurTextures[i].get() };
		const RTexture* to[1] = { m_blurTextures[i + 1].get() };
		m_pRenderer->Compute(Graphics::DOWN_BLUR_CS, L"DOWN BLUR", to, _countof(to), from, _countof(from), samplers, _countof(samplers), nullptr, group_x / static_cast<UINT>(std::pow(2, i + 1)) + 1, group_y / static_cast<UINT>(std::pow(2, i + 1)) + 1, 1);
	}

	// swap m_bloomTextures[0] with SourceTexture again, Then after Upsample, SourceTexture has original texture, m_bloomTextures[0] has processed texture
	m_blurTextures[0].swap(m_renderTargetToProcess);

	// Upsample Blur
	for (size_t i = LEVEL; i > 0; --i)
	{
		const RTexture* from[1] = { m_blurTextures[i].get() };
		const RTexture* to[1] = { m_blurTextures[i - 1].get() };

		m_pRenderer->Compute(Graphics::UP_BLUR_CS, L"UP BLUR", to, _countof(to), from, _countof(from), samplers, _countof(samplers), nullptr, group_x / static_cast<UINT>(std::pow(2, i - 1)) + 1, group_y / static_cast<UINT>(std::pow(2, i - 1)) + 1, 1);
	}
	// now has blurred texture
	m_renderTargetProcessed.swap(m_blurTextures[0]);
}