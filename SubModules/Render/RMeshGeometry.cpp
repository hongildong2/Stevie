#include "pch.h"
#include "RMeshGeometry.h"

RMeshGeometry::RMeshGeometry(const EPrimitiveTopologyType type, const EMeshType meshType)
	: IRenderResource()
	, m_meshType(meshType)
	, m_topologyType(type)
	, m_indexCount()
	, m_indexFormat()
	, m_vertexOffset()
	, m_vertexStride()
{
}
