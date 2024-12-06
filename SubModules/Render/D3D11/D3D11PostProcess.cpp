#include "pch.h"
#include "D3D11Texture.h"
#include "D3D11Resources.h"
#include "D3D11MeshGeometry.h"
#include "D3D11DeviceResources.h"
#include "Core/GeometryGenerator.h"
#include "../RPostProcess.h"
#include "../RResourceManager.h"
#include "../RenderConfig.h"
#include "../RMeshGeometry.h"

RPostProcess::RPostProcess()
	: m_pRenderer(nullptr)
	, m_postProcessConstant(DEFAULT_POST_PROCESS_PARAM)
	, m_postProcessCB()
{
	m_postProcessCB = std::make_unique<RBuffer>();
}

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
	m_screenQuad = std::unique_ptr<RMeshGeometry>(m_pRenderer->CreateBasicMeshGeometry(EBasicMeshGeometry::QUAD));

	UINT renderTargetWidth = m_pRenderer->GetBackBufferWidth();
	UINT renderTargetHeight = m_pRenderer->GetBackBufferHeight();

	m_renderTargetToProcess = std::unique_ptr<RTexture>(pResourceManager->CreateTextureRender(renderConfig::HDR_PIPELINE_FORMAT, renderTargetWidth, renderTargetHeight));
	m_renderTargetProcessed = std::unique_ptr<RTexture>(pResourceManager->CreateTextureRender(renderConfig::HDR_PIPELINE_FORMAT, renderTargetWidth, renderTargetHeight));

	pResourceManager->CreateConstantBuffer(sizeof(PostProcessConstant), &m_postProcessConstant, m_postProcessCB.get());

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
	m_pRenderer->GetResourceManager()->UpdateConstantBuffer(sizeof(PostProcessConstant), &m_postProcessConstant, m_postProcessCB.get());

	// release DSV, RTV from context
	auto* pContext = m_pRenderer->GetDeviceResources()->GetD3DDeviceContext();
	pContext->OMSetRenderTargets(0, NULL, NULL);

	m_pRenderer->GetDeviceResources()->PIXBeginEvent(L"PostProccess");
}

void RPostProcess::Process()
{
	ProcessFog();

	// Expected to be last
	ProcessBloom();
}

void RPostProcess::EndPostProcess()
{
	auto* pContext = m_pRenderer->GetDeviceResources()->GetD3DDeviceContext();
	auto* pRTV = m_pRenderer->GetDeviceResources()->GetRenderTargetView();

	ID3D11RenderTargetView* rtv[1] = { pRTV };
	ID3D11ShaderResourceView* srvs[2] = { m_renderTargetProcessed->GetSRV(), m_renderTargetToProcess->GetSRV() };
	pContext->PSSetConstantBuffers(5, 1, m_postProcessCB->GetAddressOf());
	pContext->PSSetShaderResources(0, 2, srvs);
	pContext->OMSetRenderTargets(1, rtv, NULL);
	pContext->VSSetShader(Graphics::QUAD_VS->Get(), NULL, NULL);
	pContext->PSSetSamplers(0, 1, Graphics::LINEAR_CLAMP_SS->GetAddressOf());
	pContext->PSSetShader(Graphics::FILTER_COMBINE_PS->Get(), NULL, NULL);
	pContext->IASetInputLayout(Graphics::SAMPLING_IL->Get());
	pContext->RSSetState(Graphics::SOLID_CW_RS->Get());



	DrawScreenQuad();
	ID3D11ShaderResourceView* release[6] = { NULL, };
	pContext->PSSetShaderResources(0, 2, release);

	m_pRenderer->GetDeviceResources()->PIXEndEvent();
}

void RPostProcess::ProcessFog()
{
	auto* pContext = m_pRenderer->GetDeviceResources()->GetD3DDeviceContext();

	auto* resultRenderTarget = m_renderTargetProcessed->GetRTV();
	auto* depthSRV = m_pRenderer->GetDeviceResources()->GetDepthSRV();

	ID3D11ShaderResourceView* srvs[2] = { m_renderTargetToProcess->GetSRV(), depthSRV };


	pContext->VSSetShader(Graphics::QUAD_VS->Get(), NULL, NULL);
	pContext->PSSetShader(Graphics::FOG_PS->Get(), NULL, NULL);
	pContext->IASetInputLayout(Graphics::SAMPLING_IL->Get());
	pContext->RSSetState(Graphics::SOLID_CW_RS->Get());
	pContext->PSSetShaderResources(0, 2, srvs);
	pContext->PSSetConstantBuffers(5, 1, m_postProcessCB->GetAddressOf());
	pContext->PSSetSamplers(0, 1, Graphics::LINEAR_CLAMP_SS->GetAddressOf());
	pContext->OMSetRenderTargets(1, &resultRenderTarget, NULL);

	DrawScreenQuad();

	ID3D11ShaderResourceView* release[6] = { NULL, };
	pContext->PSSetShaderResources(0, 2, release);
	pContext->OMSetRenderTargets(0, NULL, NULL);
	m_renderTargetProcessed.swap(m_renderTargetToProcess);
}

void RPostProcess::ProcessBloom()
{
	auto* pContext = m_pRenderer->GetDeviceResources()->GetD3DDeviceContext();
	pContext->VSSetShader(NULL, NULL, NULL);
	pContext->PSSetShader(NULL, NULL, NULL);
	ID3D11ShaderResourceView* release[1] = { NULL };
	ID3D11UnorderedAccessView* releaseUAV[1] = { NULL };

	// TODO :: Use RRenderer->Compute
	UINT renderTargetWidth = m_pRenderer->GetBackBufferWidth();
	UINT renderTargetHeight = m_pRenderer->GetBackBufferHeight();

	unsigned int group_x = static_cast<unsigned int>(ceil(renderTargetWidth / 32.f));
	unsigned int group_y = static_cast<unsigned int>(ceil(renderTargetHeight / 32.f));

	pContext->CSSetSamplers(0, 1, Graphics::LINEAR_CLAMP_SS->GetAddressOf());
	pContext->CSSetShader(Graphics::DOWN_BLUR_CS->Get(), NULL, NULL);

	// swap m_bloomTextures[0] with SourceTexture
	m_blurTextures[0].swap(m_renderTargetToProcess);

	for (size_t i = 0; i < LEVEL; ++i)
	{
		ID3D11ShaderResourceView* from = m_blurTextures[i]->GetSRV();
		ID3D11UnorderedAccessView* to = m_blurTextures[i + 1]->GetUAV();
		pContext->CSSetShaderResources(0, 1, &from);
		pContext->CSSetUnorderedAccessViews(0, 1, &to, NULL);

		pContext->Dispatch(group_x / static_cast<UINT>(std::pow(2, i + 1)) + 1, group_y / static_cast<UINT>(std::pow(2, i + 1)) + 1, 1);
		pContext->CSSetShaderResources(0, 1, release);
		pContext->CSSetUnorderedAccessViews(0, 1, releaseUAV, NULL);
	}

	// swap m_bloomTextures[0] with SourceTexture again, Then after Upsample, SourceTexture has original texture, m_bloomTextures[0] has processed texture
	m_blurTextures[0].swap(m_renderTargetToProcess);

	// Upsample Blur
	pContext->CSSetShader(Graphics::UP_BLUR_CS->Get(), NULL, NULL);
	for (size_t i = LEVEL; i > 0; --i)
	{
		ID3D11ShaderResourceView* from = m_blurTextures[i]->GetSRV();
		ID3D11UnorderedAccessView* to = m_blurTextures[i - 1]->GetUAV();
		pContext->CSSetShaderResources(0, 1, &from);
		pContext->CSSetUnorderedAccessViews(0, 1, &to, NULL);

		pContext->Dispatch(group_x / static_cast<UINT>(std::pow(2, i - 1)) + 1, group_y / static_cast<UINT>(std::pow(2, i - 1)) + 1, 1);
		pContext->CSSetShaderResources(0, 1, release);
		pContext->CSSetUnorderedAccessViews(0, 1, releaseUAV, NULL);
	}

	pContext->CSSetShader(NULL, NULL, NULL);
	// now has blurred texture
	m_renderTargetProcessed.swap(m_blurTextures[0]);
}

void RPostProcess::DrawScreenQuad()
{
	// TODO :: RRenderer::DrawScreenQuad(VS,PS,PSResource,PSSamplers,RenderTexture,PSConstant);
	auto* pContext = m_pRenderer->GetDeviceResources()->GetD3DDeviceContext();
	RMeshGeometry* pMG = m_screenQuad.get();
	ID3D11Buffer* vbf[1] = { pMG->GetVertexBuffer() };
	UINT vS = pMG->GetVertexStride();
	UINT vO = pMG->GetVertexOffset();
	pContext->IASetVertexBuffers(0, 1, vbf, &vS, &vO);
	pContext->IASetIndexBuffer(pMG->GetIndexBuffer(), pMG->GetIndexFormat(), 0);
	pContext->IASetPrimitiveTopology(GetD3D11TopologyType(pMG->GetTopologyType()));

	pContext->DrawIndexed(pMG->GetIndexCount(), 0, 0);
}
