#include "pch.h"

#include "Model.h"
#include "Utility.h"


Model::Model(const char* name, const EModelType type, GraphicsPSO& pso)
	: AObject(name, EObjectType::MODEL),
	IGUIComponent(EGUIType::MODEL),
	m_type(type),
	m_PSO(pso),
	m_world(),
	m_worldPos()
{
	m_meshes.reserve(10);
	AObject::SetComponentFlag(EComponentsFlag::GUI);
}

AObject* Model::GetThis()
{
	return this;
}

void Model::AddMeshComponent(std::unique_ptr<MeshPart> pMesh)
{
	m_meshes.push_back(std::move(pMesh));
}

void Model::Initialize(ID3D11Device1* pDevice)
{
	assert(pDevice != nullptr);

	for (std::unique_ptr<MeshPart>& meshs : m_meshes)
	{
		meshs->Initialize(pDevice);
	}
}

void Model::Update(ID3D11DeviceContext1* pContext)
{
	assert(pContext != nullptr);

	// ¾îÂ¼±¸ ÀúÂ¼±¸

	for (std::unique_ptr<MeshPart>& meshs : m_meshes)
	{
		meshs->Prepare(pContext, m_world);
	}
}

void Model::Render(ID3D11DeviceContext1* pContext)
{
	assert(pContext != nullptr);

	Graphics::SetPipelineState(pContext, m_PSO);

	for (std::unique_ptr<MeshPart>& meshs : m_meshes)
	{
		meshs->Draw(pContext);
	}
}

void Model::RenderOverride(ID3D11DeviceContext1* pContext, const GraphicsPSO& pso)
{
	assert(pContext != nullptr);

	Graphics::SetPipelineState(pContext, pso);

	for (std::unique_ptr<MeshPart>& mesh : m_meshes)
	{
		mesh->Draw(pContext);
	}
}
void Model::UpdatePosByTransform(const DirectX::SimpleMath::Matrix& deltaTransform)
{
	m_world *= deltaTransform;
}

void Model::UpdatePosByCoordinate(const DirectX::SimpleMath::Vector4 pos)
{
	DirectX::SimpleMath::Vector4 diff = m_worldPos - pos;
	UpdatePosByTransform(DirectX::SimpleMath::Matrix::CreateTranslation({ diff.x, diff.y, diff.z }));
	m_worldPos = pos;
}