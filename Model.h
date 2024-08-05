#pragma once
#include "pch.h"

#include <vector>
#include <memory>

#include "MeshPart.h"
#include "ModelCommon.h"
#include "EModelType.h"
#include "IGUIComponent.h"
#include "AObject.h"

// TODO : ��� GUI������Ʈ�� Most Derived Class���� ��ӹ޴°� �´°� ����. �װ� �� �򰥸��� ����. -> Sphere, Plane �� ����� GUIŸ�� ���� ����
class Model : public AObject, public IGUIComponent
{
public:
	Model(const char* name, const EModelType type, GraphicsPSO& pso);
	virtual ~Model() = default;

	Model(const Model& other) = delete;
	Model& operator=(const Model& other) = delete;

	const AObject* GetThis() const override;

	void AddMeshComponent(std::unique_ptr<MeshPart> pMesh);

	virtual void Initialize(ID3D11Device1* pDevice);

	virtual void Update(ID3D11DeviceContext1* pContext);

	virtual void Render(ID3D11DeviceContext1* pContext);
	virtual void RenderOverride(ID3D11DeviceContext1* pContext, const GraphicsPSO& pso);


	// Rotation, Scale, update component..
	void UpdatePosByTransform(const DirectX::SimpleMath::Matrix& deltaTransform);
	void UpdatePosByCoordinate(const DirectX::SimpleMath::Vector4 pos);

	inline const EModelType GetType() const
	{
		return m_type;
	}

	inline DirectX::SimpleMath::Matrix GetWorldMatrix() const
	{
		return m_world;
	}

	inline DirectX::SimpleMath::Vector4 GetWorldPos() const
	{
		return m_worldPos;
	}

	inline const std::string& GetName() const
	{
		return m_name;
	}

protected:
	const std::string m_name;
	const EModelType m_type;
	std::vector<std::unique_ptr<MeshPart>> m_meshes;

	DirectX::SimpleMath::Vector4 m_worldPos;
	DirectX::SimpleMath::Matrix m_world;
	// row pitch yaw

	GraphicsPSO m_PSO;
};

