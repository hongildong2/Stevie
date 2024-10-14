#pragma once
#include "pch.h"

class SGameObject;

class MeshComponent final
{
public:
	MeshComponent() = default;
	virtual ~MeshComponent();

	void Initialize(RMeshGeometry* pMeshGeometry, RMaterial* pMaterial);

	inline const RMeshGeometry* GetMeshGeometry() const
	{
		return m_pMeshGeometry;
	}

	inline const RMaterial* GetMaterial() const
	{
		return m_pMaterial;
	}



private:
	const RMeshGeometry* m_pMeshGeometry;
	const RMaterial* m_pMaterial;
};

