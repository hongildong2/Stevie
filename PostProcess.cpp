#include "pch.h"

#include "PostProcess.h"
#include "Utility.h"
#include "MeshPart.h"

PostProcess::PostProcess(const RECT size)
	:m_originalSize(size),
	m_postProcessConstant(DEFAULT_POST_PROCESS_PARAM)
{
	m_bloomTextures.reserve(static_cast<size_t>(LEVEL + 1));

}
void PostProcess::Initialize(ID3D11Device1* device)
{
	MeshData quad = GeometryGenerator::MakeSquare(1.f);
	m_screenQuad = std::make_unique<MeshPart>(quad, device);


	RECT textureSizeByLevel = m_originalSize;

	for (int i = 0; i <= LEVEL; ++i)
	{
		std::unique_ptr<RenderTexture> buffer = std::make_unique<RenderTexture>(DXGI_FORMAT_R16G16B16A16_FLOAT);
		buffer->SetDevice(device);
		buffer->SetWindow(textureSizeByLevel);
		m_bloomTextures.push_back(std::move(buffer));
		textureSizeByLevel.left /= 2;
		textureSizeByLevel.right /= 2;
		textureSizeByLevel.top /= 2;
		textureSizeByLevel.bottom /= 2;
	}

	Utility::DXResource::CreateConstantBuffer(m_postProcessConstant, device, m_postProcessCB);
}

void PostProcess::FillTextureToProcess(ID3D11DeviceContext1* pContext, ID3D11Texture2D* pRenderedBuffer)
{
	pContext->OMSetRenderTargets(0, NULL, NULL); // to release texture2D from RTV

	assert(pRenderedBuffer != nullptr);
	pContext->CopyResource(m_textureToProcess->GetRenderTarget(), pRenderedBuffer);
}

void PostProcess::ProcessBloom(ID3D11DeviceContext1* context)
{
	unsigned int group_x = static_cast<unsigned int>(ceil(m_originalSize.right / 32.f));
	unsigned int group_y = static_cast<unsigned int>(ceil(m_originalSize.bottom / 32.f));

	context->CSSetSamplers(0, 1, Graphics::linearClampSS.GetAddressOf());
	Graphics::SetPipelineState(context, Graphics::downBlurPSO);

	// first bloom texture size should be same with render size
	// Bloom은 원본도 있어야한다..
	context->CopyResource(m_bloomTextures[0]->GetRenderTarget(), m_textureToProcess->GetRenderTarget());

	for (size_t i = 0; i < LEVEL; ++i)
	{
		ID3D11ShaderResourceView* from = m_bloomTextures[i]->GetShaderResourceView();
		ID3D11UnorderedAccessView* to = m_bloomTextures[i + 1]->GetUnorderedAccessView();
		context->CSSetShaderResources(0, 1, &from);
		context->CSSetUnorderedAccessViews(0, 1, &to, NULL);

		context->Dispatch(group_x / static_cast<UINT>(std::pow(2, i + 1)) + 1, group_y / static_cast<UINT>(std::pow(2, i + 1)) + 1, 1);

		Utility::ComputeShaderBarrier(context);
	}

	// Upsample Blur
	Graphics::SetPipelineState(context, Graphics::upBlurPSO);
	for (size_t i = LEVEL; i > 0; --i)
	{
		ID3D11ShaderResourceView* from = m_bloomTextures[i]->GetShaderResourceView();
		ID3D11UnorderedAccessView* to = m_bloomTextures[i - 1]->GetUnorderedAccessView();
		context->CSSetShaderResources(0, 1, &from);
		context->CSSetUnorderedAccessViews(0, 1, &to, NULL);

		context->Dispatch(group_x / static_cast<UINT>(std::pow(2, i - 1)) + 1, group_y / static_cast<UINT>(std::pow(2, i - 1)) + 1, 1);
		Utility::ComputeShaderBarrier(context);
	}
}

void PostProcess::ProcessFog(ID3D11DeviceContext1* pContext, ID3D11ShaderResourceView* depthOnlySRV)
{
	auto* rtvToDraw = m_textureProcessed->GetRenderTargetView();
	pContext->OMSetRenderTargets(1, &rtvToDraw, NULL);

	ID3D11ShaderResourceView* SRVs[2] = { m_textureToProcess->GetShaderResourceView(), depthOnlySRV };
	pContext->PSSetShaderResources(100, 2, SRVs);

	Graphics::SetPipelineState(pContext, Graphics::depthOnlyPSO);
	m_screenQuad->Draw(pContext);

	ID3D11ShaderResourceView* release[6] = { NULL, };
	pContext->PSSetShaderResources(100, 6, release);
	pContext->OMSetRenderTargets(0, NULL, NULL);

	m_textureToProcess.swap(m_textureProcessed); // 다음 후처리할 친구위해
}

void PostProcess::Draw(ID3D11DeviceContext1* context, ID3D11RenderTargetView* rtvToDraw)
{
	// texture to process에 처리된 결과물 있음
	Graphics::SetPipelineState(context, Graphics::filterCombinePSO);
	Utility::DXResource::UpdateConstantBuffer(m_postProcessConstant, context, m_postProcessCB);
	context->PSSetConstantBuffers(0, 1, m_postProcessCB.GetAddressOf());

	auto* bloomed = m_bloomTextures[0]->GetShaderResourceView();
	auto* fogged = m_textureToProcess->GetShaderResourceView();
	ID3D11ShaderResourceView* combineSRVs[2] = { bloomed, fogged };
	context->PSSetShaderResources(100, 2, combineSRVs);

	context->OMSetRenderTargets(1, &rtvToDraw, NULL);

	Graphics::SetPipelineState(context, Graphics::filterCombinePSO);
	m_screenQuad->Draw(context);

	ID3D11ShaderResourceView* release[6] = { 0, };
	context->PSSetShaderResources(0, 6, release);
}

void PostProcess::UpdateConstant(PostProcessConstant constant)
{
	m_postProcessConstant = constant;
}

