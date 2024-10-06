#pragma once
#include "pch.h"

namespace Graphics
{
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

	// Samplers
	extern RSamplerState* linearWrapSS;
	extern RSamplerState* linearClampSS;
	extern RSamplerState* linearMirrorSS;
	extern RSamplerState* shadowPointSS;
	extern RSamplerState* shadowCompareSS;

	extern RRasterizerState* basicRS;
	extern RRasterizerState* solidCcwRS;
	extern RRasterizerState* wireframeCcwRS;
	extern RRasterizerState* wireframeCwRS;

	// Depth Stencil States
	// extern ComPtr<ID3D11DepthStencilState> basicDSS; // 일반적으로 그리기


	// Shaders
	extern RVertexShader* basicVS;
	extern RVertexShader* cubemapVS; // namespace cubemap
	extern RVertexShader* screenQuadVS;
	extern RVertexShader* depthOnlyVS;


	extern RPixelShader* basicPS;
	extern RPixelShader* cubemapPS;
	extern RPixelShader* filterCombinePS;
	extern RPixelShader* fogPS;
	extern RPixelShader* volumePS;


	extern RComputeShader* downBlurCS;
	extern RComputeShader* upBlurCS;
	extern RComputeShader* cloudDensityCS;
	extern RComputeShader* cloudLightingCS;


	extern RHullShader* tessellatedQuadHS;
	extern RHullShader* tessellatedQuadDepthOnlyHS;
	extern RDomainShader* tessellatedQuadDS;

	// Input Layouts
	extern RInputLayout* basicIL;
	extern RInputLayout* samplingIL;

	extern RBlendState* alphaBS;


	namespace Ocean
	{
		extern RComputeShader* initialSpectrumCS;
		extern RComputeShader* timedependentSpectrumCS;
		extern RComputeShader* FFTCS;
		extern RComputeShader* FFTPostProcessCS;
		extern RComputeShader* combineWaveCS;
		extern RComputeShader* foamSimulationCS;

		extern RPixelShader* oceanPS;
	}


	namespace D3D11
	{
		void InitCommonResources(ID3D11Device1* pDevice);
		void InitShaders(ID3D11Device* pDevice);
		void InitSamplerStates(ID3D11Device* pDevice);
		void InitBlendStates(ID3D11Device* pDevice);
		void InitSamplerStates(ID3D11Device* pDevice);
		void InitRasterizerStates(ID3D11Device* pDevice);
	}

	// namespace D3D12



	void ClearCommonResources(); // Delete All
}

