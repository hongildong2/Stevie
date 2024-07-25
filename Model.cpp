#include "pch.h"

#include "Model.h"
#include "Utility.h"

Model::Model(const char* name, std::vector<std::unique_ptr<ModelMeshPart>>&& meshes, DirectX::SimpleMath::Vector3 worldPosition)
	:m_name(name),
	m_meshes(std::move(meshes)), // Note that name,expression of rvalue reference is lvalue
	m_modelPSConstants{ {0.f, 0.f, 0.f}, 0.f, {0.f, } },
	m_modelVSConstants{}
{
	m_meshes.reserve(50);
	m_modelVSConstants.worldMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(worldPosition);

	m_modelPSConstants.material.metallicFactor = 0.7f;
	m_modelPSConstants.material.roughnessFactor = 0.3f;
	m_modelPSConstants.material.aoFactor = 1.f;
	m_modelPSConstants.material.t1 = 1.f;
}

void Model::PrepareForRendering(ID3D11DeviceContext1* context,
	const DirectX::SimpleMath::Matrix& viewMatrix, const DirectX::SimpleMath::Matrix& projMatrix, const DirectX::SimpleMath::Vector3& eyeWorld)
{
	m_modelVSConstants.viewMatrix = viewMatrix.Transpose();
	m_modelVSConstants.projMatrix = projMatrix.Transpose();
	m_modelVSConstants.worldMatrix = GetWorldMatrix().Transpose();
	m_modelVSConstants.worldMatrixIT = m_modelVSConstants.worldMatrix.Invert().Transpose();

	m_modelPSConstants.eyeWorld = eyeWorld;

	Utility::DXResource::UpdateConstantBuffer(m_modelVSConstants, context, m_VSConstantsBuffer);
	Utility::DXResource::UpdateConstantBuffer(m_modelPSConstants, context, m_PSConstantBuffer);
	context->VSSetConstantBuffers(0, 1, m_VSConstantsBuffer.GetAddressOf());
	context->PSSetConstantBuffers(2, 1, m_PSConstantBuffer.GetAddressOf());

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
	context->VSSetShaderResources(0, 1, textures + 2);
	context->PSSetShaderResources(4, 6, textures);
}


void Model::Initialize(Microsoft::WRL::ComPtr<ID3D11Device1> device, TextureFiles files)
{
	assert(device != nullptr);
	m_modelVSConstants.projMatrix = DirectX::SimpleMath::Matrix();
	m_modelVSConstants.viewMatrix = DirectX::SimpleMath::Matrix();
	m_modelVSConstants.worldMatrix = DirectX::SimpleMath::Matrix();
	m_modelVSConstants.worldMatrixIT = DirectX::SimpleMath::Matrix();

	// 버퍼도 사실 동일한 타입이면 공유해도 되긴함 일단 이렇게
	Utility::DXResource::CreateConstantBuffer(m_modelVSConstants, device, m_VSConstantsBuffer);
	Utility::DXResource::CreateConstantBuffer(m_modelPSConstants, device, m_PSConstantBuffer);

	if (files.albedoName == nullptr)
	{
		return;
	}
	// TODO : Create AssetManaget, get view from it
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
void Model::UpdatePosBy(const DirectX::SimpleMath::Matrix& deltaTransform)
{
	m_modelVSConstants.worldMatrix *= deltaTransform;
	m_modelVSConstants.worldMatrixIT = m_modelVSConstants.worldMatrix.Invert().Transpose();
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