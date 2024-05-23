#include "pch.h"
#include "Model.h"
#include "Utility.h"

Model::Model(const char* name, std::vector<ModelMeshPart>& meshes, DirectX::SimpleMath::Vector3 worldPosition) : m_name(name), m_WorldPosition(worldPosition)
{
	m_meshes.reserve(50);

	for (ModelMeshPart& mesh : meshes)
	{
		m_meshes.push_back(mesh);
	}
}

void Model::PrepareForRendering(ID3D11DeviceContext1* context,
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterState,
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout,
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader,
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader,
	const DirectX::SimpleMath::Matrix& viewMatrix, const DirectX::SimpleMath::Matrix& projMatrix, const DirectX::SimpleMath::Vector3& eyeWorld)
{
	assert(rasterState != nullptr && inputLayout != nullptr && vertexShader != nullptr && pixelShader != nullptr);
	context->IASetInputLayout(inputLayout.Get());

	context->VSSetShader(vertexShader.Get(), NULL, 0);
	context->PSSetShader(pixelShader.Get(), NULL, 0);

	context->RSSetState(rasterState.Get());


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
	context->PSSetShaderResources(0, 1, m_textureView.GetAddressOf());
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

void Model::Update()
{

}

DirectX::SimpleMath::Matrix Model::GetWorldMatrix() const
{
	return DirectX::SimpleMath::Matrix::CreateTranslation(m_WorldPosition);
}