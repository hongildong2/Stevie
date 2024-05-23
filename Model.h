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

struct Material
{
	DirectX::SimpleMath::Vector3 ambient = DirectX::SimpleMath::Vector3(0.0f);  // 12
	float shininess = 0.01f;           // 4
	DirectX::SimpleMath::Vector3 diffuse = DirectX::SimpleMath::Vector3(0.0f);  // 12
	float dummy1 = 0.f;                     // 4
	DirectX::SimpleMath::Vector3 specular = DirectX::SimpleMath::Vector3(1.0f); // 12
	float dummy2 = 0.f;                     // 4
	DirectX::SimpleMath::Vector3 fresnelR0 = DirectX::SimpleMath::Vector3(1.0f, 0.71f, 0.29f); // Gold
	float dummy3 = 0.f;
};

// TODO get texture views from manager when Init using name!
//struct Textures
//{
//	std::string albedo;
//	std::string metalness;
//};

static_assert(sizeof(Material) % 16 == 0, "Mesh Material Alignment");


struct PSConstants
{
	DirectX::SimpleMath::Vector3 eyeWorld;
	bool useTexture;
	Material material;
	DirectX::SimpleMath::Vector3 rimColor = DirectX::SimpleMath::Vector3(1.0f);
	float rimPower;
	float rimStrength = 0.0f;
	bool useSmoothstep = false;
	float dummy[2] = { 0, };
};

static_assert(sizeof(PSConstants) % 16 == 0, "PSConstants Alignment");


// 메쉬역할도 동시에 하는중
class Model
{
public:
	Model(const char* name, std::vector<ModelMeshPart>& meshes, DirectX::SimpleMath::Vector3 worldPosition);

	// ㄱㄴ?
	~Model() = default; // TODO : 포인터들 전부 nullptr, reset호출은 매니저에서
	Model& operator=(const Model& other) = delete;

	// TODO : PSO로 바꾸기
	// TODO : MVP Matrix 정보 Actor로 나중에 옮기기
	void PrepareForRendering(ID3D11DeviceContext1* context,
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterState,
		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout,
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader,
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader,
		const DirectX::SimpleMath::Matrix& viewMatrix, const DirectX::SimpleMath::Matrix& projMatrix, const DirectX::SimpleMath::Vector3& eyeWorld);
	void Draw(ID3D11DeviceContext1* context);

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device1> device, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView);

	// Rotation, Scale, update component..
	void Update();

	DirectX::SimpleMath::Matrix GetWorldMatrix() const;

private:
	const std::string m_name;
	std::vector<ModelMeshPart> m_meshes;

	DirectX::SimpleMath::Vector3 m_WorldPosition;

	// constant buffer for model
	VSConstants m_modelVSConstants;
	PSConstants m_modelPSConstants;

	// TODO : View, Proj는 다른 상수버퍼로 분리
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VSConstantsBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_PSConstantBuffer;

	// TODO : 이렇게 리소스뷰를 그대로 던져주는게아니라 텍스쳐 이름 두고 매니져 두고 리소스 매니져에서 가져오는게 맞지않을까?? Init할때 한번만, 프로퍼티로 텍스쳐이름만 넣고
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureView;
};

