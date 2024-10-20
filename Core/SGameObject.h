#pragma once
#include "pch.h"
#include "SObject.h"

class MeshComponent;

class SGameObject final : public SObject
{
public:
	SGameObject();
	~SGameObject();

	virtual void Initialize();
	virtual void SetMeshComponent(MeshComponent* pMeshComponent);
	virtual void Render();
	virtual void Update();

	void UpdatePos(DirectX::SimpleMath::Vector3 deltaPos);


private:
	DirectX::SimpleMath::Matrix GetWorldRowMat() const;

private:
	MeshComponent* m_pMeshComponent;

	DirectX::SimpleMath::Vector3 m_posWorld;
	float m_rollEuler;
	float m_pitchEuler;
	float m_yawEuler;
};

