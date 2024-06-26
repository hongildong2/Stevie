#include "pch.h"
#include "PostProcess.h"
#include "Utility.h"
#include "ModelMeshPart.h"

void PostProcess::Initialize(ID3D11Device1* device, const RECT size)
{
	m_textures.reserve(LEVEL + 1);
	m_textures.clear();
	m_originalSize = size;

	std::vector<ModelMeshPart> meshes;
	MeshData quad = GeometryGenerator::MakeSquare();
	ModelMeshPart quadMesh = ModelMeshPart(quad, device);
	meshes.push_back(quadMesh);
	m_screenQuad = std::make_unique<Model>("Screen Quad", meshes, DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f));

	RECT textureSizeByLevel = m_originalSize;

	for (int i = 0; i <= LEVEL; ++i)
	{
		std::unique_ptr<RenderTexture> buffer = std::make_unique<RenderTexture>(DXGI_FORMAT_R16G16B16A16_FLOAT);
		buffer->SetDevice(device);
		buffer->SetWindow(textureSizeByLevel);
		m_textures.push_back(std::move(buffer));
		textureSizeByLevel.left /= 2;
		textureSizeByLevel.right /= 2;
		textureSizeByLevel.top /= 2;
		textureSizeByLevel.bottom /= 2;
	}

	Utility::DXResource::CreateConstantBuffer(m_postProcessConstant, device, m_postProcessCB);
}

void PostProcess::Process(ID3D11DeviceContext1* context)
{
	// denominators should be consistent with compute shader's thread numbers in group
	// TODO : #define direction in CS?
	unsigned int group_x = ceil(m_originalSize.right / 32.f);
	unsigned int group_y = ceil(m_originalSize.bottom / 32.f);

	context->CSSetSamplers(0, 1, Graphics::linearClampSS.GetAddressOf());
	Graphics::SetPipelineState(context, Graphics::downBlurPSO);
	// Downsmaple Blur
	for (unsigned int i = 0; i < LEVEL; ++i)
	{
		ID3D11ShaderResourceView* from = m_textures[i]->GetShaderResourceView();
		ID3D11UnorderedAccessView* to = m_textures[i + 1]->GetUnorderedAccessView();
		context->CSSetShaderResources(0, 1, &from);
		context->CSSetUnorderedAccessViews(0, 1, &to, NULL);

		context->Dispatch(group_x / std::pow(2, i + 1) + 1, group_y / std::pow(2, i + 1) + 1, 1);

		Utility::ComputeShaderBarrier(context);
	}
	// Upsample Blur

	Graphics::SetPipelineState(context, Graphics::upBlurPSO);
	for (unsigned int i = LEVEL; i > 0; --i)
	{
		ID3D11ShaderResourceView* from = m_textures[i]->GetShaderResourceView();
		ID3D11UnorderedAccessView* to = m_textures[i - 1]->GetUnorderedAccessView();
		context->CSSetShaderResources(0, 1, &from);
		context->CSSetUnorderedAccessViews(0, 1, &to, NULL);

		context->Dispatch(group_x / std::pow(2, i - 1) + 1, group_y / std::pow(2, i - 1) + 1, 1);
		Utility::ComputeShaderBarrier(context);
	}
}

void PostProcess::Draw(ID3D11DeviceContext1* context)
{
	Graphics::SetPipelineState(context, Graphics::filterCombinePSO);
	Utility::DXResource::UpdateConstantBuffer(m_postProcessConstant, context, m_postProcessCB);
	context->PSSetConstantBuffers(0, 1, m_postProcessCB.GetAddressOf());

	m_screenQuad->Draw(context);
}

void PostProcess::UpdateConstant(PostProcessConstant constant)
{
	m_postProcessConstant = constant;
}

