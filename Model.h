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


// 메쉬역할도 동시에 하는중
class Model
{
public:
	// 이건 그냥 답이없음, 액터 좀 추가하다가 완전히 갈아없어야됨
	Model(const char* name, std::vector<std::unique_ptr<ModelMeshPart>>&& meshes, GraphicsPSO& pso);
	Model(const Model& other) = delete;

	~Model() = default;
	Model& operator=(const Model& other) = delete;


	// TODO : MVP Matrix 정보 Actor로 나중에 옮기기
	void PrepareForRendering(ID3D11DeviceContext1* context,
		const DirectX::SimpleMath::Matrix& viewMatrix, const DirectX::SimpleMath::Matrix& projMatrix, const DirectX::SimpleMath::Vector3& eyeWorld);
	void Draw(ID3D11DeviceContext1* context);

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device1> device, TextureFiles files);

	// Rotation, Scale, update component..
	void UpdatePosBy(const DirectX::SimpleMath::Matrix& deltaTransform);

	void UpdateMaterialConstant(Material& mat);
	Material GetMaterialConstant() const;

	DirectX::SimpleMath::Matrix GetWorldMatrix() const;

	// 죄송합니다
	ID3D11Buffer* GetVSCB() const;

private:
	const std::string m_name;
	std::vector<std::unique_ptr<ModelMeshPart>> m_meshes;

	// constant buffer for model
	VSConstants m_modelVSConstants;

	// TODO : 여긴 최대한 모델에 관련된 정보만 넣고 월드나 픽셀쉐이더의 쉐이더 자체 정보는 다른 버퍼에 있어야지. 버퍼분리하자.
	PSConstants m_modelPSConstants;

	GraphicsPSO m_PSO;

	// TODO : View, Proj는 다른 상수버퍼로 분리
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VSConstantsBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_PSConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_albedoView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_aoview;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_heightView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_metallicView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_roughnessView;
};

