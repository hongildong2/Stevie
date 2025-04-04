#pragma once
#include "pch.h"

class RInputLayout;
class RSamplerState;
class RRasterizerState;

class RVertexShader;
class RPixelShader;
class RComputeShader;
class RDomainShader;
class RHullShader;
class RGeometryShader;

class RBlendState;
class RDepthStencilState;

class RRenderer;

namespace Graphics
{

	// Shaders
	extern RVertexShader* BASIC_VS;
	extern RVertexShader* CUBEMAP_VS;
	extern RVertexShader* QUAD_VS;
	extern RVertexShader* DEPTH_ONLY_VS;
	extern RVertexShader* SAMPLING_VS;


	extern RPixelShader* BASIC_PS;
	extern RPixelShader* CUBEMAP_PS;
	extern RPixelShader* FILTER_COMBINE_PS;
	extern RPixelShader* FOG_PS;
	extern RPixelShader* VOLUME_PS;
	extern RPixelShader* OCEAN_SURFACE_PS;
	extern RPixelShader* DEMO_PS;



	extern RComputeShader* DOWN_BLUR_CS;
	extern RComputeShader* UP_BLUR_CS;
	extern RComputeShader* CLOUD_DENSITY_CS;
	extern RComputeShader* CLOUD_LIGHTING_CS;
	extern RComputeShader* OCEAN_INITIAL_SPECTRUM_CS;
	extern RComputeShader* OCEAN_TIME_DEPENDENT_SPECTRUM_CS;
	extern RComputeShader* OCEAN_FFT_CS;
	extern RComputeShader* OCEAN_FFT_POST_PROCESS_CS;
	extern RComputeShader* OCEAN_COMBINE_WAVE_CS;
	extern RComputeShader* OCEAN_FOAM_SIMULATION_CS;


	extern RHullShader* TESSELLATED_QUAD_HS;
	extern RDomainShader* TESSELATED_QUAD_DS;


	// Samplers
	extern RSamplerState* LINEAR_WRAP_SS;
	extern RSamplerState* LINEAR_CLAMP_SS;
	extern RSamplerState* LINEAR_MIRROR_SS;
	extern RSamplerState* SHADOW_POINT_SS;
	extern RSamplerState* SHADOW_COMPARE_SS;

	extern RRasterizerState* SOLID_CW_RS;
	extern RRasterizerState* SOLID_CCW_RS;
	extern RRasterizerState* WIRE_FRAME_CW_RS;
	extern RRasterizerState* WIRE_FRAME_CCW_RS;

	// Input Layouts
	extern RInputLayout* BASIC_IL;
	extern RInputLayout* SAMPLING_IL;

	extern RBlendState* ALPHA_BS;

	extern RDepthStencilState* SKYBOX_DSS;
	extern RDepthStencilState* OPAQUE_DSS;
	extern RDepthStencilState* TRANSPARENT_DSS;
	extern RDepthStencilState* SCREEN_QUAD_DSS;


	void InitCommonResources(const RRenderer* pRenderer);
	void InitShaders(const RRenderer* pRenderer);
	void InitSamplerStates(const RRenderer* pRenderer);
	void InitBlendStates(const RRenderer* pRenderer);
	void InitRasterizerStates(const RRenderer* pRenderer);
	void InitDepthStencilStates(const RRenderer* pRenderer);
	void CheckValidity();
	void ClearCommonResources(); // Delete All
}

