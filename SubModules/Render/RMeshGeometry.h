#pragma once
#include "RenderDefs.h"

class RMeshGeometry : public IRenderResource
{
public:
	RMeshGeometry(const EPrimitiveTopologyType type);
	~RMeshGeometry() = default;

	inline EPrimitiveTopologyType GetTopologyType() const
	{
		return m_topologyType;
	}

	virtual void Draw(const XMMATRIX* pWorld) = 0;


protected:
	const EPrimitiveTopologyType m_topologyType;

	// bIsTessellated; -> Set DS, HS, and its resource,
	// bIsGeometric;

	UINT m_vertexStride;
	UINT m_vertexOffset;
	UINT m_indexCount;
	DXGI_FORMAT m_indexFormat;
};

