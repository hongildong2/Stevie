#pragma once
#include "pch.h"
#include "SObject.h"

class MeshComponent;

class SGameObject final : public SObject
{
public:
	SGameObject();
	~SGameObject();

	virtual void Initialize(IRenderer* pRenderer, MeshComponent* pMeshComponent);
	virtual void Render();
	virtual void Update();


private:
	DirectX::SimpleMath::Matrix GetModelTransform() const
	{
		DirectX::XMMatrixRotationRollPitchYaw(m_pitchEuler, m_yawEuler, m_rollEuler)* DirectX::XMMatrixTranslation(m_worldPos.x, m_worldPos.y, m_worldPos.z);
	}

private:
	MeshComponent* m_pMeshComponent;
	IRenderer* m_pRenderer;

	DirectX::SimpleMath::Vector4 m_worldPos;
	float m_rollEuler;
	float m_pitchEuler;
	float m_yawEuler;
};

