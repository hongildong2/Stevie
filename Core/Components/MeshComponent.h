#pragma once
#include "pch.h"
class MeshComponent
{
	// RMeshGeometry :: vertices, vs, gs, ...
	// Material :: textures, ps ...
public:
	inline const RMeshGeometry* GetMeshGeometry() const
	{
		return m_pMeshGeometry;
	}

	inline const RMaterial* GetMaterial() const
	{
		return m_pMaterial;
	}

private:
	RMeshGeometry* m_pMeshGeometry;
	RMaterial* m_pMaterial;
};

