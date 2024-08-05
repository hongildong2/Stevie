#pragma once
#include "GraphicsCommon.h"
#include "Utility.h"
#include "ModelCommon.h"
#include "EMeshType.h"
#include "IGUIComponent.h"
/*
* reference : https://github.com/microsoft/DirectXTK/wiki/ModelMeshPart
*
*/

class MeshPart final
{
public:
	MeshPart(MeshData& mesh, const EMeshType meshType, ID3D11Device1* pDevice, const TextureFiles& tex);
	~MeshPart() = default;

	MeshPart(const MeshPart& other) = delete;
	MeshPart& operator=(const MeshPart& other) = delete;

	MeshPart(const MeshPart&& other) = delete;
	MeshPart& operator=(const MeshPart&& other) = delete;

	void Initialize(ID3D11Device1* pDevice);

	void Prepare(ID3D11DeviceContext1* pContext, DirectX::SimpleMath::Matrix& parentWorld);

	void Draw(ID3D11DeviceContext1* pContext) const;

	void UpdateMaterialConstant(Material& mat);
	void UpdateMeshConstant(MeshConstants& mesh);

	inline ID3D11Buffer* GetMeshCB() const
	{
		return m_meshCB.Get();
	}

	inline ID3D11Buffer* GetMaterialCB() const
	{
		return m_materialCB.Get();
	}


private:
	// relative to Model
	DirectX::SimpleMath::Vector3 m_modelPos;
	// m_modelRot

	const EMeshType m_type;

	const UINT m_vertexStride;
	const UINT m_vertexOffset;
	const UINT m_indexCount;

	const DXGI_FORMAT m_indexFormat;

	MeshConstants m_meshConstants;
	Material m_materialConstants;


	Microsoft::WRL::ComPtr<ID3D11Buffer> m_meshCB;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_materialCB;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_albedoView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_aoview;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_heightView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_metallicView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_roughnessView;
};

