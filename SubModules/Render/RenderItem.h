#pragma once
#include "pch.h"

struct RenderParam
{
	UINT data[64];
};

static_assert((sizeof(RenderParam) % 16) == 0);

template<typename T>
FORCEINLINE RenderParam* CAST_RENDER_PARAM_PTR(T* a)
{
	static_assert(sizeof(T) <= sizeof(RenderParam), "GIVEN RENDER PARAMETER TOO LARGE");
	return reinterpret_cast<RenderParam*>(a);
}

template<typename U>
FORCEINLINE void MEMCPY_RENDER_PARAM(RenderParam* dst, U* src)
{
	static_assert(sizeof(U) <= sizeof(RenderParam), "GIVEN RENDER PARAMETER TOO LARGE");
	std::memcpy(dst, src, sizeof(U));
}



struct RenderItem
{
	const class RMeshGeometry* pMeshGeometry;
	const class RMaterial* pMaterial;

	// Draw Policy
	const class RBlendState* pBlendState;
	bool bIsOccluder;
	bool bIsTransparent;
	Vector4 blendFactor;

	RenderParam meshParam;
	RenderParam materialParam;
};