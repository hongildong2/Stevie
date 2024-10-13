#pragma once
#include "pch.h"
class MeshComponent final
{
	// RMeshGeometry :: vertices, vs, gs, ...
	// Material :: textures, ps ...
public:
	MeshComponent() = default;
	virtual ~MeshComponent();

	void Initialize(RMeshGeometry* pMeshGeometry, RMaterial* pMaterial);
	void SetRelativePos(XMFLOAT4 deltaPos);

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
	XMFLOAT4 m_modelPos;
};

