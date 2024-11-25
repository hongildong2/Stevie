#include "pch.h"
#include "D3D11Renderer.h"
#include "D3D11ResourceManager.h"
#include "D3D11PostProcess.h"
#include "D3D11Texture.h"
#include "D3D11Resources.h"
#include "D3D11MeshGeometry.h"
#include "Core/GeometryGenerator.h"
#include "../RenderConfig.h"
#include "../RMeshGeometry.h"

void D3D11PostProcess::Initialize(D3D11Renderer* pRenderer)
{
	MY_ASSERT(pRenderer != nullptr);
	{
		pQuadVS = static_cast<D3D11VertexShader*>(Graphics::QUAD_VS);
		pSamplingIL = static_cast<D3D11InputLayout*>(Graphics::SAMPLING_IL);
		pFogPS = static_cast<D3D11PixelShader*>(Graphics::FOG_PS);
		pFilterCombinePS = static_cast<D3D11PixelShader*>(Graphics::FILTER_COMBINE_PS);
		pBasicRS = static_cast<D3D11RasterizerState*>(Graphics::SOLID_CW_RS);

		pUpBlurCS = static_cast<D3D11ComputeShader*>(Graphics::UP_BLUR_CS);
		pDownBlurCS = static_cast<D3D11ComputeShader*>(Graphics::DOWN_BLUR_CS);
		pLinearClampSS = static_cast<D3D11SamplerState*>(Graphics::LINEAR_CLAMP_SS);
	}

	if (m_pRenderer != nullptr)
	{
		m_renderTargetProcessed.reset();
		m_renderTargetToProcess.reset();
		m_blurTextures.clear();
	}

	m_pRenderer = pRenderer;
	auto* pResourceManager = pRenderer->GetResourceManager();

	m_postProcessConstant = DEFAULT_POST_PROCESS_PARAM;

	m_screenQuad = std::unique_ptr<RMeshGeometry>(m_pRenderer->CreateBasicMeshGeometry(EBasicMeshGeometry::QUAD));

	UINT renderTargetWidth = m_pRenderer->GetBackBufferWidth();
	UINT renderTargetHeight = m_pRenderer->GetBackBufferHeight();

	m_renderTargetToProcess = std::unique_ptr<D3D11TextureRender>(pResourceManager->CreateTextureRender(renderConfig::HDR_PIPELINE_FORMAT, renderTargetWidth, renderTargetHeight));
	m_renderTargetProcessed = std::unique_ptr<D3D11TextureRender>(pResourceManager->CreateTextureRender(renderConfig::HDR_PIPELINE_FORMAT, renderTargetWidth, renderTargetHeight));

	pResourceManager->CreateConstantBuffer(sizeof(PostProcessConstant), &m_postProcessConstant, m_postProcessCB.ReleaseAndGetAddressOf());

	UINT blurRTWidth = renderTargetWidth;
	UINT blurRTHeight = renderTargetHeight;
	for (UINT i = 0; i <= LEVEL; ++i)
	{
		m_blurTextures.emplace_back(std::unique_ptr<D3D11TextureRender>(pResourceManager->CreateTextureRender(renderConfig::HDR_PIPELINE_FORMAT, blurRTWidth, blurRTHeight)));
		blurRTWidth /= 2;
		blurRTHeight /= 2;
	}

}

void D3D11PostProcess::BeginPostProcess(std::unique_ptr<D3D11TextureRender>& sourceRenderTarget)
{
	m_renderTargetToProcess.swap(sourceRenderTarget);
	m_pRenderer->GetResourceManager()->UpdateConstantBuffer(sizeof(PostProcessConstant), &m_postProcessConstant, m_postProcessCB.Get());

	// release DSV, RTV from context
	auto* pContext = m_pRenderer->GetDeviceResources()->GetD3DDeviceContext();
	pContext->OMSetRenderTargets(0, NULL, NULL);

	m_pRenderer->GetDeviceResources()->PIXBeginEvent(L"PostProccess");
}

void D3D11PostProcess::Process()
{
	ProcessFog();

	// Expected to be last
	ProcessBloom();
}

void D3D11PostProcess::EndPostProcess(ID3D11RenderTargetView* pRTVToDraw)
{
	auto* pContext = m_pRenderer->GetDeviceResources()->GetD3DDeviceContext();

	ID3D11ShaderResourceView* srvs[2] = { m_renderTargetProcessed->GetSRVOrNull(), m_renderTargetToProcess->GetSRVOrNull() };
	pContext->PSSetConstantBuffers(5, 1, m_postProcessCB.GetAddressOf());
	pContext->PSSetShaderResources(0, 2, srvs);
	pContext->OMSetRenderTargets(1, &pRTVToDraw, NULL);
	pContext->VSSetShader(pQuadVS->Get(), NULL, NULL);
	pContext->PSSetSamplers(0, 1, pLinearClampSS->GetAddressOf());
	pContext->PSSetShader(pFilterCombinePS->Get(), NULL, NULL);
	pContext->IASetInputLayout(pSamplingIL->Get());
	pContext->RSSetState(pBasicRS->Get());



	DrawScreenQuad();
	ID3D11ShaderResourceView* release[6] = { NULL, };
	pContext->PSSetShaderResources(0, 2, release);

	m_pRenderer->GetDeviceResources()->PIXEndEvent();
}

void D3D11PostProcess::ProcessFog()
{
	auto* pContext = m_pRenderer->GetDeviceResources()->GetD3DDeviceContext();

	auto* resultRenderTarget = m_renderTargetProcessed->GetRTV();
	auto* depthSRV = m_pRenderer->GetDeviceResources()->GetDepthSRV();

	ID3D11ShaderResourceView* srvs[2] = { m_renderTargetToProcess->GetSRVOrNull(), depthSRV };


	pContext->VSSetShader(pQuadVS->Get(), NULL, NULL);
	pContext->PSSetShader(pFogPS->Get(), NULL, NULL);
	pContext->IASetInputLayout(pSamplingIL->Get());
	pContext->RSSetState(pBasicRS->Get());
	pContext->PSSetShaderResources(0, 2, srvs);
	pContext->PSSetConstantBuffers(5, 1, m_postProcessCB.GetAddressOf());
	pContext->PSSetSamplers(0, 1, pLinearClampSS->GetAddressOf());
	pContext->OMSetRenderTargets(1, &resultRenderTarget, NULL);

	DrawScreenQuad();

	ID3D11ShaderResourceView* release[6] = { NULL, };
	pContext->PSSetShaderResources(0, 2, release);
	pContext->OMSetRenderTargets(0, NULL, NULL);
	m_renderTargetProcessed.swap(m_renderTargetToProcess);
}

void D3D11PostProcess::ProcessBloom()
{
	auto* pContext = m_pRenderer->GetDeviceResources()->GetD3DDeviceContext();
	pContext->VSSetShader(NULL, NULL, NULL);
	pContext->PSSetShader(NULL, NULL, NULL);
	ID3D11ShaderResourceView* release[1] = { NULL };
	ID3D11UnorderedAccessView* releaseUAV[1] = { NULL };

	UINT renderTargetWidth = m_pRenderer->GetBackBufferWidth();
	UINT renderTargetHeight = m_pRenderer->GetBackBufferHeight();

	unsigned int group_x = static_cast<unsigned int>(ceil(renderTargetWidth / 32.f));
	unsigned int group_y = static_cast<unsigned int>(ceil(renderTargetHeight / 32.f));

	pContext->CSSetSamplers(0, 1, pLinearClampSS->GetAddressOf());
	pContext->CSSetShader(pDownBlurCS->Get(), NULL, NULL);

	// swap m_bloomTextures[0] with SourceTexture
	m_blurTextures[0].swap(m_renderTargetToProcess);

	for (size_t i = 0; i < LEVEL; ++i)
	{
		ID3D11ShaderResourceView* from = m_blurTextures[i]->GetSRVOrNull();
		ID3D11UnorderedAccessView* to = m_blurTextures[i + 1]->GetUAVOrNull();
		pContext->CSSetShaderResources(0, 1, &from);
		pContext->CSSetUnorderedAccessViews(0, 1, &to, NULL);

		pContext->Dispatch(group_x / static_cast<UINT>(std::pow(2, i + 1)) + 1, group_y / static_cast<UINT>(std::pow(2, i + 1)) + 1, 1);
		pContext->CSSetShaderResources(0, 1, release);
		pContext->CSSetUnorderedAccessViews(0, 1, releaseUAV, NULL);
	}

	// swap m_bloomTextures[0] with SourceTexture again, Then after Upsample, SourceTexture has original texture, m_bloomTextures[0] has processed texture
	m_blurTextures[0].swap(m_renderTargetToProcess);

	// Upsample Blur
	pContext->CSSetShader(pUpBlurCS->Get(), NULL, NULL);
	for (size_t i = LEVEL; i > 0; --i)
	{
		ID3D11ShaderResourceView* from = m_blurTextures[i]->GetSRVOrNull();
		ID3D11UnorderedAccessView* to = m_blurTextures[i - 1]->GetUAVOrNull();
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

void D3D11PostProcess::DrawScreenQuad()
{
	auto* pContext = m_pRenderer->GetDeviceResources()->GetD3DDeviceContext();
	D3D11MeshGeometry* pMG = static_cast<D3D11MeshGeometry*>(m_screenQuad.get());
	ID3D11Buffer* vbf[1] = { pMG->GetVertexBuffer() };
	UINT vS = pMG->GetVertexStride();
	UINT vO = pMG->GetVertexOffset();
	pContext->IASetVertexBuffers(0, 1, vbf, &vS, &vO);
	pContext->IASetIndexBuffer(pMG->GetIndexBuffer(), pMG->GetIndexFormat(), 0);
	pContext->IASetPrimitiveTopology(GetD3D11TopologyType(pMG->GetTopologyType()));

	pContext->DrawIndexed(pMG->GetIndexCount(), 0, 0);
}
