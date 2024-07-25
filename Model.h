#pragma once
#include "pch.h"

#include <vector>
#include <memory>

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

struct TextureFiles
{
	const wchar_t* albedoName;
	const wchar_t* aoName;
	const wchar_t* heightName;
	const wchar_t* metallicName;
	const wchar_t* normalName;
	const wchar_t* roughnessName;
};

struct Material
{
	float metallicFactor;
	float aoFactor;
	float roughnessFactor;
	float t1;
};


struct PSConstants
{
	DirectX::SimpleMath::Vector3 eyeWorld;
	float dummy;
	Material material;
};

static_assert(sizeof(PSConstants) % 16 == 0, "PSConstants Alignment");


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
	void UpdatePosBy(const DirectX::SimpleMath::Matrix& deltaTransform);

	void UpdateMaterialConstant(Material& mat);
	Material GetMaterialConstant() const;

	DirectX::SimpleMath::Matrix GetWorldMatrix() const;

	// �˼��մϴ�
	ID3D11Buffer* GetVSCB() const;

private:
	const std::string m_name;
	std::vector<std::unique_ptr<ModelMeshPart>> m_meshes;

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

