#pragma once
#include "pch.h"

#include <vector>
#include <memory>

#include "ModelMeshPart.h"
#include "ModelCommon.h"

// �޽����ҵ� ���ÿ� �ϴ���
class Model
{
public:
	// �̰� �׳� ���̾���, ���� �� �߰��ϴٰ� ������ ���ƾ���ߵ�
	Model(const char* name, std::vector<std::unique_ptr<ModelMeshPart>>&& meshes, GraphicsPSO& pso);
	Model(const Model& other) = delete;

	~Model() = default;
	Model& operator=(const Model& other) = delete;


	// TODO : MVP Matrix ���� Actor�� ���߿� �ű��
	void PrepareForRendering(ID3D11DeviceContext1* context,
		const DirectX::SimpleMath::Matrix& viewMatrix, const DirectX::SimpleMath::Matrix& projMatrix, const DirectX::SimpleMath::Vector3& eyeWorld);
	void Draw(ID3D11DeviceContext1* context);

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device1> device, TextureFiles files);

	// Rotation, Scale, update component..
	void UpdatePosByTransform(const DirectX::SimpleMath::Matrix& deltaTransform);
	void UpdatePosByCoordinate(const DirectX::SimpleMath::Vector4 pos);

	void UpdateMaterialConstant(Material& mat);
	Material GetMaterialConstant() const;

	DirectX::SimpleMath::Matrix GetWorldMatrix() const;

	// �˼��մϴ�
	inline ID3D11Buffer* GetVSCB() const
	{
		return m_VSConstantsBuffer.Get();
	}

	inline ID3D11Buffer* GetPSCB() const
	{
		return m_PSConstantBuffer.Get();
	}

	inline DirectX::SimpleMath::Vector4 GetWorldPos() const
	{
		return m_worldPos;
	}

private:
	const std::string m_name;
	std::vector<std::unique_ptr<ModelMeshPart>> m_meshes;
	DirectX::SimpleMath::Vector4 m_worldPos;

	// constant buffer for model
	VSConstants m_modelVSConstants;

	// TODO : ���� �ִ��� �𵨿� ���õ� ������ �ְ� ���峪 �ȼ����̴��� ���̴� ��ü ������ �ٸ� ���ۿ� �־����. ���ۺи�����.
	PSConstants m_modelPSConstants;

	GraphicsPSO m_PSO;

	// TODO : View, Proj�� �ٸ� ������۷� �и�
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VSConstantsBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_PSConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_albedoView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_aoview;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_heightView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_metallicView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_roughnessView;
};

