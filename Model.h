#pragma once
#include "pch.h"
#include <vector>
#include "ModelMeshPart.h"

class Model
{
public:
	Model(const char* name, std::vector<ModelMeshPart>& meshes);

	// ����?
	~Model() = default;
	Model& operator=(const Model& other) = default;

	// TODO : PSO�� �ٲٱ�
	// TODO : MVP Matrix ���� Actor�� ���߿� �ű��
	void PrepareForRendering(ID3D11DeviceContext1* context,
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterState,
		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout,
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader,
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader,
		DirectX::SimpleMath::Matrix viewMatrix, DirectX::SimpleMath::Matrix projMatrix) const;

	void Draw(ID3D11DeviceContext1* context);

	void Update();

private:
	const std::string m_name;
	std::vector<ModelMeshPart> m_meshes;

	DirectX::SimpleMath::Vector4 worldPosition;
	DirectX::SimpleMath::Matrix worldMatrix;
	// Model Matrix
};

