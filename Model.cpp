#include "pch.h"
#include "Model.h"
#include "Utility.h"

Model::Model(const char* name, std::vector<ModelMeshPart>& meshes, DirectX::SimpleMath::Vector3 worldPosition, Microsoft::WRL::ComPtr<ID3D11Device1> device) : m_name(name), m_WorldPosition(worldPosition)
{
	m_meshes.reserve(50);

	for (ModelMeshPart& mesh : meshes)
	{
		m_meshes.push_back(mesh);
	}


	m_VSConstantsCPU.projMatrix = DirectX::SimpleMath::Matrix();
	m_VSConstantsCPU.viewMatrix = DirectX::SimpleMath::Matrix();
	m_VSConstantsCPU.worldMatrix = DirectX::SimpleMath::Matrix();
	m_VSConstantsCPU.worldMatrixIT = DirectX::SimpleMath::Matrix();

	Utility::DXResource::CreateConstantBuffer(m_VSConstantsCPU, device, m_VSConstantsBufferGPU);
}


void Model::PrepareForRendering(ID3D11DeviceContext1* context,
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterState,
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout,
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader,
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader,
	const DirectX::SimpleMath::Matrix& viewMatrix, const DirectX::SimpleMath::Matrix& projMatrix)
{
	assert(rasterState != nullptr && inputLayout != nullptr && vertexShader != nullptr && pixelShader != nullptr);
	context->IASetInputLayout(inputLayout.Get());

	context->VSSetShader(vertexShader.Get(), NULL, 0);
	context->PSSetShader(pixelShader.Get(), NULL, 0);

	context->RSSetState(rasterState.Get());


	// TODO : View, Proj는 다른 상수버퍼로 분리
	m_VSConstantsCPU.viewMatrix = viewMatrix.Transpose();
	m_VSConstantsCPU.projMatrix = projMatrix.Transpose();
	m_VSConstantsCPU.worldMatrix = GetWorldMatrix().Transpose();
	m_VSConstantsCPU.worldMatrixIT = m_VSConstantsCPU.worldMatrix.Invert().Transpose();

	Utility::DXResource::UpdateConstantBuffer(m_VSConstantsCPU, context, m_VSConstantsBufferGPU);

	context->VSSetConstantBuffers(0, 1, m_VSConstantsBufferGPU.GetAddressOf());
}

// Expecting PrepareForRendering has done
void Model::Draw(ID3D11DeviceContext1* context)
{
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