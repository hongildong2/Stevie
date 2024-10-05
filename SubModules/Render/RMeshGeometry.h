#pragma once
#include "IRenderer.h"

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
	// DrawTessellated(DS, HS, constantBuffers, textureResources);
	// DrawGeometry(GS, constant buffers, resources)

protected:
	const EPrimitiveTopologyType m_topologyType;

	UINT m_vertexStride;
	UINT m_vertexOffset;
	UINT m_indexCount;
	DXGI_FORMAT m_indexFormat;
};

