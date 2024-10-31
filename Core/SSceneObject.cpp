#include "pch.h"
#include "SSceneObject.h"
#include "Core/Components/MeshComponent.h"

using namespace DirectX::SimpleMath;

SSceneObject::SSceneObject()
	: m_pMeshComponent()
	, m_pitchEuler()
	, m_yawEuler()
	, m_rollEuler()
	, m_posWorld()
{
}

SSceneObject::~SSceneObject()
{
	// delete m_pMeshComponent;
}

void SSceneObject::Initialize()
{
}

void SSceneObject::SetMeshComponent(MeshComponent* pMeshComponent)
{
	m_pMeshComponent = pMeshComponent;
}

void SSceneObject::Render()
{
	if (m_pMeshComponent != nullptr)
	{
		m_pMeshComponent->Render(GetWorldRowMat());
	}
}

void SSceneObject::Update()
{
}

Matrix SSceneObject::GetWorldRowMat() const
{
	return Matrix::CreateFromYawPitchRoll(m_yawEuler, m_pitchEuler, m_rollEuler) * Matrix::CreateTranslation(m_posWorld);
}

void SSceneObject::UpdatePos(DirectX::SimpleMath::Vector3 deltaPos)
{
	m_posWorld += deltaPos;
}


