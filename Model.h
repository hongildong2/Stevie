#pragma once
#include "pch.h"
#include <vector>
#include "ModelMeshPart.h"

// temp
struct VSConstants
{
	DirectX::SimpleMath::Matrix worldMatrix;
	DirectX::SimpleMath::Matrix worldMatrixIT;
	DirectX::SimpleMath::Matrix viewMatrix;
	DirectX::SimpleMath::Matrix projMatrix;
};
static_assert(sizeof(VSConstants) % 16 == 0, "Constant Buffer Alignment");


class Model
{
public:
	Model(const char* name, std::vector<ModelMeshPart>& meshes, DirectX::SimpleMath::Vector3 worldPosition, Microsoft::WRL::ComPtr<ID3D11Device1> device);

	// ㄱㄴ?
	~Model() = default;
	Model& operator=(const Model& other) = default;

	// TODO : PSO로 바꾸기
	// TODO : MVP Matrix 정보 Actor로 나중에 옮기기
	void PrepareForRendering(ID3D11DeviceContext1* context,
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterState,
		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout,
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader,
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader,
		const DirectX::SimpleMath::Matrix& viewMatrix, const DirectX::SimpleMath::Matrix& projMatrix);

	void Draw(ID3D11DeviceContext1* context);

	void Update();

	DirectX::SimpleMath::Matrix GetWorldMatrix() const;
	// Rotation, Scale



	// Reset on shutdown
private:
	const std::string m_name;
	std::vector<ModelMeshPart> m_meshes;

	DirectX::SimpleMath::Vector3 m_WorldPosition;

	// constant buffer for model
	VSConstants m_VSConstantsCPU;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VSConstantsBufferGPU;
};

