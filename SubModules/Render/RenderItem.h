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
	RMeshGeometry* pMeshGeometry;
	RMaterial* pMaterial;
	RenderParam objectParam;
	RenderParam drawParam;
};