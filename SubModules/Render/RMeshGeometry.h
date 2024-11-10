#pragma once
#include "pch.h"
#include "IRenderResource.h"

enum class EMeshType
{
	BASIC,
	TESSELLATED_QUAD
};

class RMeshGeometry : public IRenderResource
{
public:
	RMeshGeometry(const EPrimitiveTopologyType type, const EMeshType meshType);
	~RMeshGeometry() = default;

	inline EPrimitiveTopologyType GetTopologyType() const
	{
		return m_topologyType;
	}

	virtual void Draw() const = 0;


protected:
	const EPrimitiveTopologyType m_topologyType;
	const EMeshType m_meshType;

	UINT m_vertexStride;
	UINT m_vertexOffset;
	UINT m_indexCount;
	DXGI_FORMAT m_indexFormat;
};

