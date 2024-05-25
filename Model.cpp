#include "pch.h"
#include "Model.h"
#include "Utility.h"

Model::Model(const char* name, std::vector<ModelMeshPart>& meshes, DirectX::SimpleMath::Vector3 worldPosition) : m_name(name)
{
	m_meshes.reserve(50);
	m_modelVSConstants.worldMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(worldPosition);
	m_modelPSConstants.material.ambient = DirectX::SimpleMath::Vector3(0.6f);
	m_modelPSConstants.material.diffuse = DirectX::SimpleMath::Vector3(0.6f);

	for (ModelMeshPart& mesh : meshes)
	{
		m_meshes.push_back(mesh);
	}
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


	// 0번에 MVP, 1번에 Light, 2번에 PSConstant. 이거는 어떻게 관리하지?
	context->VSSetConstantBuffers(0, 1, m_VSConstantsBuffer.GetAddressOf());
	context->PSSetConstantBuffers(2, 1, m_PSConstantBuffer.GetAddressOf());
	context->PSSetShaderResources(1, 1, m_textureView.GetAddressOf());
}


void Model::Initialize(Microsoft::WRL::ComPtr<ID3D11Device1> device, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView)
{
	assert(device != nullptr && textureView != nullptr);
	m_modelVSConstants.projMatrix = DirectX::SimpleMath::Matrix();
	m_modelVSConstants.viewMatrix = DirectX::SimpleMath::Matrix();
	m_modelVSConstants.worldMatrix = DirectX::SimpleMath::Matrix();
	m_modelVSConstants.worldMatrixIT = DirectX::SimpleMath::Matrix();

	// 버퍼도 사실 동일한 타입이면 공유해도 되긴함 일단 이렇게
	Utility::DXResource::CreateConstantBuffer(m_modelVSConstants, device, m_VSConstantsBuffer);
	Utility::DXResource::CreateConstantBuffer(m_modelPSConstants, device, m_PSConstantBuffer);


	// TODO : 이렇게 리소스뷰를 그대로 던져주는게아니라 텍스쳐 이름 두고 매니져 두고 리소스 매니져에서 가져오는게 맞지않을까?? Init할때 한번만, 프로퍼티로 텍스쳐이름만 넣고
	// 생성자에서 텍스쳐 이름 등등 받고 매니져에서 뷰 가져오기
	m_textureView = textureView;
}
// Expecting PrepareForRendering has done
void Model::Draw(ID3D11DeviceContext1* context)
{
	assert(context != nullptr);

	for (ModelMeshPart& mesh : m_meshes)
	{
		mesh.Draw(context);
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