#include "pch.h"

#include "Model.h"
#include "Utility.h"

Model::Model(const char* name, std::vector<std::unique_ptr<ModelMeshPart>>&& meshes, GraphicsPSO& pso)
	:m_name(name),
	m_meshes(std::move(meshes)), // Note that name,expression of rvalue reference is lvalue
	m_modelPSConstants{ {0.f, 0.f, 0.f}, 0.f, DEFAULT_MATERIAL },
	m_modelVSConstants{ DirectX::SimpleMath::Matrix(), DirectX::SimpleMath::Matrix(), DirectX::SimpleMath::Matrix(), DirectX::SimpleMath::Matrix() },
	m_PSO(pso)
{
	m_meshes.reserve(10);
}

void Model::PrepareForRendering(ID3D11DeviceContext1* context,
	const DirectX::SimpleMath::Matrix& viewMatrix, const DirectX::SimpleMath::Matrix& projMatrix, const DirectX::SimpleMath::Vector3& eyeWorld)
{

	// 진짜 한방에 다 고칠게요
	VSConstants toSend;

	toSend.viewMatrix = viewMatrix.Transpose();
	toSend.projMatrix = projMatrix.Transpose();
	toSend.worldMatrix = GetWorldMatrix().Transpose();
	toSend.worldMatrixIT = m_modelVSConstants.worldMatrixIT.Invert().Transpose();


	m_modelPSConstants.eyeWorld = eyeWorld;

	Utility::DXResource::UpdateConstantBuffer(toSend, context, m_VSConstantsBuffer);
	Utility::DXResource::UpdateConstantBuffer(m_modelPSConstants, context, m_PSConstantBuffer);
	context->VSSetConstantBuffers(0, 1, m_VSConstantsBuffer.GetAddressOf());
	context->PSSetConstantBuffers(2, 1, m_PSConstantBuffer.GetAddressOf());

	// 밖에서 Override할 수 있게, 딱히 Override안하면 모델에 정해진대로 파이프라인 실행된다.
	Graphics::SetPipelineState(context, m_PSO);

	// 함정이..
	if (m_albedoView == nullptr)
	{
		return;
	}

	ID3D11ShaderResourceView* textures[] = {
		m_albedoView.Get(),
		m_aoview.Get(),
		m_heightView.Get(),
		m_metallicView.Get(),
		m_normalView.Get(),
		m_roughnessView.Get()
	};


	// 0번에 MVP, 1번에 Light, 2번에 PSConstant. 이거는 어떻게 관리하지?
	context->VSSetShaderResources(0, 1, textures + 2); // height map texture
	context->PSSetShaderResources(4, 6, textures);
}


void Model::Initialize(Microsoft::WRL::ComPtr<ID3D11Device1> device, TextureFiles files)
{
	assert(device != nullptr);

	// 버퍼도 사실 동일한 타입이면 공유해도 되긴함 일단 이렇게
	Utility::DXResource::CreateConstantBuffer(m_modelVSConstants, device, m_VSConstantsBuffer);
	Utility::DXResource::CreateConstantBuffer(m_modelPSConstants, device, m_PSConstantBuffer);

	if (files.albedoName == nullptr)
	{
		return;
	}

	// All Textures should be SRGB, linear space
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFileEx(device.Get(), files.albedoName, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_FLAG(false), DirectX::WIC_LOADER_DEFAULT, nullptr, m_albedoView.GetAddressOf()));
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFileEx(device.Get(), files.aoName, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_FLAG(false), DirectX::WIC_LOADER_DEFAULT, nullptr, m_aoview.GetAddressOf()));
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFileEx(device.Get(), files.heightName, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_FLAG(false), DirectX::WIC_LOADER_DEFAULT, nullptr, m_heightView.GetAddressOf()));
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFileEx(device.Get(), files.metallicName, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_FLAG(false), DirectX::WIC_LOADER_DEFAULT, nullptr, m_metallicView.GetAddressOf()));
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFileEx(device.Get(), files.normalName, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_FLAG(false), DirectX::WIC_LOADER_DEFAULT, nullptr, m_normalView.GetAddressOf()));
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFileEx(device.Get(), files.roughnessName, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_FLAG(false), DirectX::WIC_LOADER_DEFAULT, nullptr, m_roughnessView.GetAddressOf()));
}
// Expecting PrepareForRendering has done
void Model::Draw(ID3D11DeviceContext1* context)
{
	assert(context != nullptr);

	for (std::unique_ptr<ModelMeshPart>& mesh : m_meshes)
	{
		mesh->Draw(context);
	}
}
void Model::UpdatePosByTransform(const DirectX::SimpleMath::Matrix& deltaTransform)
{
	m_modelVSConstants.worldMatrix *= deltaTransform;
	m_modelVSConstants.worldMatrixIT = m_modelVSConstants.worldMatrix.Invert().Transpose();
}

void Model::UpdatePosByCoordinate(const DirectX::SimpleMath::Vector4 pos)
{
	DirectX::SimpleMath::Vector4 diff = m_worldPos - pos;
	UpdatePosByTransform(DirectX::SimpleMath::Matrix::CreateTranslation({ diff.x, diff.y, diff.z }));
	m_worldPos = pos;
}


void Model::UpdateMaterialConstant(Material& mat)
{
	m_modelPSConstants.material = mat;
}

Material Model::GetMaterialConstant() const
{
	return m_modelPSConstants.material;
}

DirectX::SimpleMath::Matrix Model::GetWorldMatrix() const
{
	return m_modelVSConstants.worldMatrix;
}