#pragma once
#include "IRenderer.h"

class PShader;

class RMeshGeometry : public IRenderResource
{
public:
private:
	const EPrimitiveTopologyType m_topologyType;
	XMMATRIX m_transform;
};

