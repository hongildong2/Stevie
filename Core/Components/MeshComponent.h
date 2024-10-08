#pragma once
#include "pch.h"
class MeshComponent
{
	// RMeshGeometry :: vertices, vs, gs, ...
	// Material :: textures, ps ...
public:
	inline RMeshGeometry* GetMeshGeometry() const
	{
		return m_pMeshGeometry;
	}

	inline RMaterial* GetMaterial() const
	{
		return m_pMaterial;
	}

private:
	RMeshGeometry* m_pMeshGeometry;
	RMaterial* m_pMaterial;
	XMMATRIX m_model;
};

