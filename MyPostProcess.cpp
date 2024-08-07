#include "pch.h"

#include "MyPostProcess.h"
#include "Utility.h"

MyPostProcess::MyPostProcess(const RECT size, DXGI_FORMAT pipelineFormat)
	: AObject("PostProcess", EObjectType::POST_PROCESS),
	IGUIComponent(EGUIType::POST_PROCESS),
	m_originalSize(size),
	m_postProcessConstant(DEFAULT_POST_PROCESS_PARAM),
	m_textureToProcess(std::make_unique<RenderTexture>(pipelineFormat)),
	m_textureProcessed(std::make_unique<RenderTexture>(pipelineFormat))
{
	m_bloomTextures.reserve(static_cast<size_t>(LEVEL + 1));
	AObject::SetComponentFlag(EComponentsFlag::GUI);
}


void MyPostProcess::Initialize(ID3D11Device1* device)
{
	MeshData quad = GeometryGenerator::MakeSquare(1.f);
	m_screenQuad = std::make_unique<MeshPart>(quad, EMeshType::SOLID, device);


	RECT textureSizeByLevel = m_originalSize;

	m_textureProcessed->SetDevice(device);
	m_textureToProcess->SetDevice(device);

	m_textureProcessed->SetWindow(m_originalSize);
	m_textureToProcess->SetWindow(m_originalSize);

	for (int i = 0; i <= LEVEL; ++i)
	{
		std::unique_ptr<RenderTexture> buffer = std::make_unique<RenderTexture>(m_textureToProcess->GetFormat());
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

AObject* MyPostProcess::GetThis()
{
	return this;
}

void MyPostProcess::FillTextureToProcess(ID3D11DeviceContext1* pContext, ID3D11Texture2D* pRenderedBuffer)
{
	pContext->OMSetRenderTargets(0, NULL, NULL); // to release texture2D from RTV

	assert(pRenderedBuffer != nullptr);
	pContext->CopyResource(m_textureToProcess->GetRenderTarget(), pRenderedBuffer);
}

void MyPostProcess::ProcessBloom(ID3D11DeviceContext1* context)
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
		context->CSSetShaderResources(100, 1, &from);
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
		context->CSSetShaderResources(100, 1, &from);
		context->CSSetUnorderedAccessViews(0, 1, &to, NULL);

		context->Dispatch(group_x / static_cast<UINT>(std::pow(2, i - 1)) + 1, group_y / static_cast<UINT>(std::pow(2, i - 1)) + 1, 1);
		Utility::ComputeShaderBarrier(context);
	}

	m_textureProcessed.swap(m_bloomTextures[0]);
}

void MyPostProcess::ProcessFog(ID3D11DeviceContext1* pContext, ID3D11ShaderResourceView* depthMapSRV)
{
	auto* rtvToDraw = m_textureProcessed->GetRenderTargetView();
	pContext->OMSetRenderTargets(1, &rtvToDraw, NULL);

	ID3D11ShaderResourceView* SRVs[2] = { m_textureToProcess->GetShaderResourceView(), depthMapSRV };
	pContext->PSSetShaderResources(100, 2, SRVs);

	Graphics::SetPipelineState(pContext, Graphics::fogPSO);
	m_screenQuad->Draw(pContext);

	ID3D11ShaderResourceView* release[6] = { NULL, };
	pContext->PSSetShaderResources(100, 6, release);
	pContext->OMSetRenderTargets(0, NULL, NULL);

	m_textureToProcess.swap(m_textureProcessed); // 다음 후처리할 친구위해
}

void MyPostProcess::Draw(ID3D11DeviceContext1* context, ID3D11RenderTargetView* rtvToDraw)
{
	Utility::DXResource::UpdateConstantBuffer(m_postProcessConstant, context, m_postProcessCB);
	context->PSSetConstantBuffers(5, 1, m_postProcessCB.GetAddressOf());

	auto* bloomed = m_textureProcessed->GetShaderResourceView();
	auto* notBloomed = m_textureToProcess->GetShaderResourceView();
	ID3D11ShaderResourceView* combineSRVs[2] = { bloomed, notBloomed };
	context->PSSetShaderResources(100, 2, combineSRVs);

	context->OMSetRenderTargets(1, &rtvToDraw, NULL);

	Graphics::SetPipelineState(context, Graphics::filterCombinePSO);
	m_screenQuad->Draw(context);

	ID3D11ShaderResourceView* release[6] = { 0, };
	context->PSSetShaderResources(100, 6, release);
}

void MyPostProcess::UpdateConstant(PostProcessConstant constant)
{
	m_postProcessConstant = constant;
}

