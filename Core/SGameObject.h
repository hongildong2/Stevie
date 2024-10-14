#pragma once
#include "pch.h"
#include "SObject.h"

class MeshComponent;

class SGameObject final : public SObject
{
public:
	SGameObject();
	~SGameObject();

	virtual void Initialize(IRenderer* pRenderer);
	virtual void SetMeshComponent(MeshComponent* pMeshComponent);
	virtual void Render();
	virtual void Update();

	DirectX::SimpleMath::Matrix GetWorldRowMat() const;


private:
	IRenderer* m_pRenderer;
	MeshComponent* m_pMeshComponent;

	DirectX::SimpleMath::Vector4 m_worldPos;
	float m_rollEuler;
	float m_pitchEuler;
	float m_yawEuler;
};

