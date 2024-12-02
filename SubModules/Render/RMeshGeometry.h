#pragma once
#include "pch.h"

#ifdef API_D3D11
#include "SubModules/Render/D3D11/D3D11MeshGeometry.h"
#define RHIMeshGeometry D3D11MeshGeometry
#endif

enum class EMeshType
{
	BASIC,
	TESSELLATED_QUAD
};

class RMeshGeometry : public RHIMeshGeometry
{
public:
	RMeshGeometry(const EPrimitiveTopologyType type, const EMeshType meshType);
	~RMeshGeometry() = default;

	void Initialize(const IRenderer* pRenderer, const void* pInVertexList, const UINT vertexSize, const UINT vertexCount, const void* pInIndexList, const UINT indexSize, const UINT indexCount);

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



private:
	const EPrimitiveTopologyType m_topologyType;
	const EMeshType m_meshType;
	const IRenderer* m_pRenderer;

	UINT m_vertexStride;
	UINT m_vertexOffset;
	UINT m_indexCount;
	DXGI_FORMAT m_indexFormat;
};

