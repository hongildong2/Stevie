#include "pch.h"
#include "RMeshGeometry.h"

RMeshGeometry::RMeshGeometry(const EPrimitiveTopologyType type, const EMeshType meshType)
	: m_meshType(meshType)
	, m_topologyType(type)
	, m_indexCount()
	, m_indexFormat()
	, m_vertexOffset()
	, m_vertexStride()
{
}
