#include "pch.h"
#include "SGameObject.h"
#include "Core/Components/MeshComponent.h"

using namespace DirectX::SimpleMath;

SGameObject::SGameObject()
	: m_pMeshComponent()
	, m_pRenderer()
	, m_pitchEuler()
	, m_yawEuler()
	, m_rollEuler()
	, m_worldPos()
{
}

SGameObject::~SGameObject()
{
	delete m_pMeshComponent;
}

void SGameObject::Initialize(IRenderer* pRenderer)
{
	m_pRenderer = pRenderer;
}

void SGameObject::SetMeshComponent(MeshComponent* pMeshComponent)
{
	m_pMeshComponent = pMeshComponent;
}

void SGameObject::Render()
{
	if (m_pMeshComponent != nullptr)
	{
		m_pRenderer->Render(m_pMeshComponent, GetWorldRowMat());
	}
}

void SGameObject::Update()
{
}

Matrix SGameObject::GetWorldRowMat() const
{
	return Matrix::CreateFromYawPitchRoll(m_yawEuler, m_pitchEuler, m_rollEuler) * Matrix::CreateTranslation({ m_worldPos.x, m_worldPos.y, m_worldPos.z });
}


