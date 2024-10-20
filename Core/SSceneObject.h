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

	void UpdatePos(DirectX::SimpleMath::Vector3 deltaPos);


private:
	DirectX::SimpleMath::Matrix GetWorldRowMat() const;

protected:
	MeshComponent* m_pMeshComponent;

	DirectX::SimpleMath::Vector3 m_posWorld;
	float m_rollEuler;
	float m_pitchEuler;
	float m_yawEuler;
};

