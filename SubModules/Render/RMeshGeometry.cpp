#include "pch.h"
#include "RMeshGeometry.h"

RMeshGeometry::RMeshGeometry(const EPrimitiveTopologyType type)
	: IRenderResource()
	, m_topologyType(type)
	, m_indexCount()
	, m_indexFormat()
	, m_vertexOffset()
	, m_vertexStride()
{
}
