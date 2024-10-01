#pragma once
#include "pch.h"

#include "GraphicsPSO.h"
#include "ComputePSO.h"
// 참고: DirectX_Graphic-Samples 미니엔진
// https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Core/GraphicsCommon.h

namespace Graphics
{

	// Samplers
	extern Microsoft::WRL::ComPtr<ID3D11SamplerState> linearWrapSS;
	extern Microsoft::WRL::ComPtr<ID3D11SamplerState> linearClampSS;
	extern Microsoft::WRL::ComPtr<ID3D11SamplerState> linearMirrorSS;
	extern Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowPointSS;
	extern Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowCompareSS;

	extern Microsoft::WRL::ComPtr<ID3D11RasterizerState> basicRS;
	extern Microsoft::WRL::ComPtr<ID3D11RasterizerState> solidCcwRS;
	extern Microsoft::WRL::ComPtr<ID3D11RasterizerState> wireframeCcwRS;
	extern Microsoft::WRL::ComPtr<ID3D11RasterizerState> wireframeCwRS;

	// Depth Stencil States
	// extern ComPtr<ID3D11DepthStencilState> basicDSS; // 일반적으로 그리기


	// Shaders
	extern Microsoft::WRL::ComPtr<ID3D11VertexShader> basicVS;
	extern Microsoft::WRL::ComPtr<ID3D11VertexShader> cubemapVS; // namespace cubemap
	extern Microsoft::WRL::ComPtr<ID3D11VertexShader> screenQuadVS;
	extern Microsoft::WRL::ComPtr<ID3D11VertexShader> depthOnlyVS;


	extern Microsoft::WRL::ComPtr<ID3D11PixelShader> basicPS;
	extern Microsoft::WRL::ComPtr<ID3D11PixelShader> cubemapPS;
	extern Microsoft::WRL::ComPtr<ID3D11PixelShader> filterCombinePS;
	extern Microsoft::WRL::ComPtr<ID3D11PixelShader> fogPS;
	extern Microsoft::WRL::ComPtr<ID3D11PixelShader> volumePS;


	extern Microsoft::WRL::ComPtr<ID3D11ComputeShader> downBlurCS;
	extern Microsoft::WRL::ComPtr<ID3D11ComputeShader> upBlurCS;
	extern Microsoft::WRL::ComPtr<ID3D11ComputeShader> cloudDensityCS;
	extern Microsoft::WRL::ComPtr<ID3D11ComputeShader> cloudLightingCS;


	extern Microsoft::WRL::ComPtr<ID3D11HullShader> tessellatedQuadHS;
	extern Microsoft::WRL::ComPtr<ID3D11HullShader> tessellatedQuadDepthOnlyHS;
	extern Microsoft::WRL::ComPtr<ID3D11DomainShader> tessellatedQuadDS;

	// Input Layouts
	extern Microsoft::WRL::ComPtr<ID3D11InputLayout> basicIL;
	extern Microsoft::WRL::ComPtr<ID3D11InputLayout> samplingIL;


	// Pipeline State Object
	extern GraphicsPSO basicPSO;
	extern GraphicsPSO cubemapPSO;
	extern GraphicsPSO filterCombinePSO;

	extern GraphicsPSO depthOnlyPSO;
	extern GraphicsPSO cubeMapDepthOnlyPSO;
	extern GraphicsPSO fogPSO;
	extern GraphicsPSO cloudPSO;


	extern ComputePSO downBlurPSO;
	extern ComputePSO upBlurPSO;
	extern ComputePSO cloudDensityPSO;
	extern ComputePSO cloudLightingPSO;

	namespace Ocean
	{
		extern Microsoft::WRL::ComPtr<ID3D11ComputeShader> initialSpectrumCS;
		extern Microsoft::WRL::ComPtr<ID3D11ComputeShader> timedependentSpectrumCS;
		extern Microsoft::WRL::ComPtr<ID3D11ComputeShader> FFTCS;
		extern Microsoft::WRL::ComPtr<ID3D11ComputeShader> FFTPostProcessCS;
		extern Microsoft::WRL::ComPtr<ID3D11ComputeShader> combineWaveCS;
		extern Microsoft::WRL::ComPtr<ID3D11ComputeShader> foamSimulationCS;

		extern Microsoft::WRL::ComPtr<ID3D11PixelShader> oceanPS;

		extern ComputePSO initialSpectrumPSO;
		extern ComputePSO timedependentSpectrumPSO;
		extern ComputePSO FFTPSO;
		extern ComputePSO FFTPostProcessPSO;
		extern ComputePSO combineWavePSO;
		extern ComputePSO foamSimulationPSO;

		extern GraphicsPSO oceanPSO;
		extern GraphicsPSO depthOnlyPSO;
	}



	// Blend States
	// extern ComPtr<ID3D11BlendState> mirrorBS;

	void InitCommonStates(ID3D11Device1* device);

	// 내부적으로 InitCommonStates()에서 사용
	void InitSamplers(Microsoft::WRL::ComPtr<ID3D11Device1> device);
	void InitRasterizerStates(ID3D11Device1* device);
	// void InitBlendStates(ComPtr<ID3D11Device>& device);
	// void InitDepthStencilStates(ComPtr<ID3D11Device>& device);
	void InitPipelineStates(Microsoft::WRL::ComPtr<ID3D11Device> device);
	void InitShaders(ID3D11Device1* device);

	// 주의: 초기화가 느려서 필요한 경우에만 초기화
	// void InitVolumeShaders(ComPtr<ID3D11Device>& device);

	void SetPipelineState(ID3D11DeviceContext1* context, const GraphicsPSO& pso);
	void SetPipelineState(ID3D11DeviceContext1* context, const ComputePSO& pso);

	HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, const D3D_SHADER_MACRO* defines, _Outptr_ ID3DBlob** blob);

} // namespace Graphics

