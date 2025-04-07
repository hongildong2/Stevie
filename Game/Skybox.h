#pragma once
#include "SSceneObject.h"
class Skybox final : public SSceneObject
{
public:
	Skybox(MeshComponent* pMeshComponent);
	~Skybox() = default;

	inline MeshComponent* GetMeshComponent()
	{
		return m_pMeshComponent;
	}
};

