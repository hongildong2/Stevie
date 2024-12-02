#pragma once
#include "pch.h"
#include "RenderConfig.h"


/* Render Param */
struct RenderParam
{
	UINT data[64];
};

static_assert((sizeof(RenderParam) % 16) == 0);

template<typename T>
FORCEINLINE RenderParam* CAST_RENDER_PARAM_PTR(T* a)
{
	static_assert(sizeof(T) <= sizeof(RenderParam), "GIVEN RENDER PARAMETER TOO LARGE");
	static_assert((sizeof(T) % 16) == 0, "CONSTANT BUFFER ALLIGNMENT");
	return reinterpret_cast<RenderParam*>(a);
}

template<typename U>
FORCEINLINE void MEMCPY_RENDER_PARAM(RenderParam* dst, U* src)
{
	static_assert(sizeof(U) <= sizeof(RenderParam), "GIVEN RENDER PARAMETER TOO LARGE");
	static_assert((sizeof(U) % 16) == 0, "CONSTANT BUFFER ALLIGNMENT");
	std::memcpy(dst, src, sizeof(U));
}


/* Render Item */
struct RenderItem
{
	const class RMeshGeometry* pMeshGeometry;

	
	/* Geometry Bindings */
	const class RTexture* ppGeometryTextures[renderLimits::MAX_RENDER_BINDINGS_COUNT];
	UINT geometryTexCount;
	const class RSamplerState* ppGeometrySamplerStates[renderLimits::MAX_RENDER_BINDINGS_COUNT];
	UINT geometrySSCount;


	/* Pixel Bindings */
	const class RPixelShader* pPS;
	const class RTexture* ppPixelTextures[renderLimits::MAX_RENDER_BINDINGS_COUNT];
	UINT pixelTexCount;
	const class RSamplerState* ppPixelSamplerStates[renderLimits::MAX_RENDER_BINDINGS_COUNT];
	UINT pixelSSCount;


	/* Draw Policy */
	const class RBlendState* pBlendState;
	bool bIsOccluder;
	bool bIsTransparent;
	Vector4 blendFactor;

	RenderParam meshParam;
	RenderParam materialParam;
};