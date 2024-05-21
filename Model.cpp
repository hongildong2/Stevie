#include "pch.h"
#include "Model.h"

Model::Model(const char* name, std::vector<ModelMeshPart>& meshes) : m_name(name)
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
	DirectX::SimpleMath::Matrix viewMatrix, DirectX::SimpleMath::Matrix projMatrix) const
{
	assert(rasterState != nullptr && inputLayout != nullptr && vertexShader != nullptr && pixelShader != nullptr);
	context->IASetInputLayout(inputLayout.Get());

	context->VSSetShader(vertexShader.Get(), NULL, 0);
	context->PSSetShader(pixelShader.Get(), NULL, 0);

	context->RSSetState(rasterState.Get());


	// set VS, PS const buffer, VP buffer
	// ������۴� �𵨿� ����, ����� ���� ������ ���� ���򰥸�
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