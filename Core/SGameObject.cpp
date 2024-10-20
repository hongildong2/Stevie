#include "pch.h"
#include "SGameObject.h"
#include "Core/Components/MeshComponent.h"

using namespace DirectX::SimpleMath;

SGameObject::SGameObject()
	: m_pMeshComponent()
	, m_pitchEuler()
	, m_yawEuler()
	, m_rollEuler()
	, m_posWorld()
{
}

SGameObject::~SGameObject()
{
	delete m_pMeshComponent;
}

void SGameObject::Initialize()
{
}

void SGameObject::SetMeshComponent(MeshComponent* pMeshComponent)
{
	m_pMeshComponent = pMeshComponent;
}

void SGameObject::Render()
{
	m_pMeshComponent->Render(GetWorldRowMat());
}

void SGameObject::Update()
{
}

Matrix SGameObject::GetWorldRowMat() const
{
	return Matrix::CreateFromYawPitchRoll(m_yawEuler, m_pitchEuler, m_rollEuler) * Matrix::CreateTranslation(m_posWorld);
}

void SGameObject::UpdatePos(DirectX::SimpleMath::Vector3 deltaPos)
{
	m_posWorld += deltaPos;
}


