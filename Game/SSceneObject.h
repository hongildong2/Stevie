#pragma once
#include "pch.h"
#include "SObject.h"

class MeshComponent;

class SSceneObject : public SObject
{
public:
	SSceneObject();
	~SSceneObject();

	virtual void Initialize();
	virtual void SetMeshComponent(MeshComponent* pMeshComponent);
	virtual void Render();
	virtual void Update();

	void UpdatePos(DirectX::SimpleMath::Vector3& deltaPos);
	void UpdateYawPitchRoll(DirectX::SimpleMath::Vector3& deltaRadian);

	inline DirectX::SimpleMath::Vector3 GetDirWorld() const
	{
		DirectX::SimpleMath::Vector3 dirWorld = GetLookAtPosWorld() - m_posWorld;
		dirWorld.Normalize();
		return dirWorld;
	}

	inline DirectX::SimpleMath::Vector3 GetLookAtPosWorld() const
	{
		return m_posWorld + m_dirModel;
	}

	inline DirectX::SimpleMath::Vector3 GetPosWorld() const
	{
		return m_posWorld;
	}

	inline DirectX::SimpleMath::Matrix GetWorldRowMat() const
	{
		return DirectX::SimpleMath::Matrix::CreateScale(m_scale) * DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(m_yawEuler, m_pitchEuler, m_rollEuler) * DirectX::SimpleMath::Matrix::CreateTranslation(m_posWorld);
	}

	inline DirectX::SimpleMath::Matrix GetViewRowMat() const
	{
		return DirectX::SimpleMath::Matrix::CreateLookAt(m_posWorld, GetLookAtPosWorld(), { 0.f,1.f,0.f }); // TODO :: Check Up Vector
	}

protected:
	MeshComponent* m_pMeshComponent;

	DirectX::SimpleMath::Vector3 m_posWorld;
	DirectX::SimpleMath::Vector3 m_dirModel;

	float m_scale;
	float m_yawEuler;
	float m_pitchEuler;
	float m_rollEuler;
};

