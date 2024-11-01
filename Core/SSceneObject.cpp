#include "pch.h"
#include "SSceneObject.h"
#include "Core/Components/MeshComponent.h"

using namespace DirectX::SimpleMath;

SSceneObject::SSceneObject()
	: m_pMeshComponent()
	, m_posWorld()
	, m_dirModel(-1.f, 0.f, 0.f)
	, m_pitchEuler()
	, m_yawEuler()
	, m_rollEuler()
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

void SSceneObject::UpdatePos(DirectX::SimpleMath::Vector3& deltaPos)
{
	m_posWorld += deltaPos;
}

void SSceneObject::UpdateYawPitchRoll(DirectX::SimpleMath::Vector3& deltaRadian)
{
	m_yawEuler -= deltaRadian.x;
	m_pitchEuler -= deltaRadian.y;
	m_rollEuler -= deltaRadian.z;

	constexpr float LIMIT = XM_PIDIV2 - 0.01f;

	m_pitchEuler = std::max(-LIMIT, m_pitchEuler);
	m_pitchEuler = std::min(+LIMIT, m_pitchEuler);

	if (m_yawEuler > XM_PI)
	{
		m_yawEuler -= XM_2PI;
	}
	else if (m_yawEuler < -XM_PI)
	{
		m_yawEuler += XM_2PI;
	}

	float y = sinf(m_pitchEuler);
	float r = cosf(m_pitchEuler);
	float z = r * cosf(m_yawEuler);
	float x = r * sinf(m_yawEuler);

	m_dirModel = Vector3(x, y, z);

}




