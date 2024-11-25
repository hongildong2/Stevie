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

	inline EMeshType GetMeshType() const
	{
		return m_meshType;
	}

	inline UINT GetIndexCount() const
	{
		return m_indexCount;
	}

	inline DXGI_FORMAT GetIndexFormat() const
	{
		return m_indexFormat;
	}

	inline UINT GetVertexStride() const
	{
		return m_vertexStride;
	}

	inline UINT GetVertexOffset() const
	{
		return m_vertexOffset;
	}

protected:
	const EPrimitiveTopologyType m_topologyType;
	const EMeshType m_meshType;

	UINT m_vertexStride;
	UINT m_vertexOffset;
	UINT m_indexCount;
	DXGI_FORMAT m_indexFormat;
};

