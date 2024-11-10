#pragma once
#include "pch.h"

struct RenderParam
{
	UINT data[127];
	UINT size;
};

static_assert((sizeof(RenderParam) % 16) == 0);


struct RenderItem
{
	const class RMeshGeometry* pMeshGeometry;
	const class RMaterial* pMaterial;

	// Draw Policy
	const class RBlendState* pBlendState;
	bool bIsOccluder;
	bool bIsTransparent;
	DirectX::SimpleMath::Vector4 blendFactor;

	RenderParam meshParam;
	RenderParam materialParam;
};