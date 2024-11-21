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

	void UpdatePos(Vector3& deltaPos);
	void UpdateYawPitchRoll(Vector3& deltaRadian);

	inline Vector3 GetDirWorld() const
	{
		Vector3 dirWorld = GetLookAtPosWorld() - m_posWorld;
		dirWorld.Normalize();
		return dirWorld;
	}

	inline Vector3 GetLookAtPosWorld() const
	{
		return m_posWorld + m_dirModel;
	}

	inline Vector3 GetPosWorld() const
	{
		return m_posWorld;
	}

	inline Matrix GetWorldRowMat() const
	{
		return Matrix::CreateScale(m_scale) * Matrix::CreateFromYawPitchRoll(m_yawEuler, m_pitchEuler, m_rollEuler) * Matrix::CreateTranslation(m_posWorld);
	}

	inline Matrix GetViewRowMat() const
	{
		return Matrix::CreateLookAt(m_posWorld, GetLookAtPosWorld(), { 0.f,1.f,0.f }); // TODO :: Check Up Vector
	}

protected:
	MeshComponent* m_pMeshComponent;

	Vector3 m_posWorld;
	Vector3 m_dirModel;

	float m_scale;

	float m_yawEuler;
	float m_pitchEuler;
	float m_rollEuler;
};

