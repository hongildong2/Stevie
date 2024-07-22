#pragma once
#include "GraphicsPSO.h"
#include "ComputePSO.h"
// ����: DirectX_Graphic-Samples �̴Ͽ���
// https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Core/GraphicsCommon.h

namespace Graphics
{

	// Samplers
	extern Microsoft::WRL::ComPtr<ID3D11SamplerState> linearWrapSS;
	extern Microsoft::WRL::ComPtr<ID3D11SamplerState> linearClampSS;
	// extern ComPtr<ID3D11SamplerState> pointClampSS;
	// extern ComPtr<ID3D11SamplerState> pointWrapSS;

	extern Microsoft::WRL::ComPtr<ID3D11RasterizerState> basicRS;
	extern Microsoft::WRL::ComPtr<ID3D11RasterizerState> solidCcwRS;

	// Depth Stencil States
	// extern ComPtr<ID3D11DepthStencilState> basicDSS; // �Ϲ������� �׸���


	// Shaders
	extern Microsoft::WRL::ComPtr<ID3D11VertexShader> basicVS;
	extern Microsoft::WRL::ComPtr<ID3D11VertexShader> cubemapVS;
	extern Microsoft::WRL::ComPtr<ID3D11VertexShader> screenQuadVS;

	extern Microsoft::WRL::ComPtr<ID3D11PixelShader> basicPS;
	extern Microsoft::WRL::ComPtr<ID3D11PixelShader> cubemapPS;
	extern Microsoft::WRL::ComPtr<ID3D11PixelShader> filterCombinePS;

	extern Microsoft::WRL::ComPtr<ID3D11ComputeShader> downBlurCS;
	extern Microsoft::WRL::ComPtr<ID3D11ComputeShader> upBlurCS;
	
	// Input Layouts
	extern Microsoft::WRL::ComPtr<ID3D11InputLayout> basicIL;
	extern Microsoft::WRL::ComPtr<ID3D11InputLayout> screenQuadIL;


	// Pipeline State Object
	extern GraphicsPSO basicPSO;
	extern GraphicsPSO cubemapPSO;
	extern GraphicsPSO filterCombinePSO;

	extern ComputePSO downBlurPSO;
	extern ComputePSO upBlurPSO;
	
	namespace Ocean
	{
		extern Microsoft::WRL::ComPtr<ID3D11ComputeShader> initialSpectrumCS;
		extern Microsoft::WRL::ComPtr<ID3D11ComputeShader> timedependentSpectrumCS;
		extern Microsoft::WRL::ComPtr<ID3D11ComputeShader> FFTCS;
		extern Microsoft::WRL::ComPtr<ID3D11ComputeShader> combineWaveCS;


		extern ComputePSO initialSpectrumPSO;
		extern ComputePSO timedependentSpectrumPSO;
		extern ComputePSO FFTPSO;
		extern ComputePSO combineWavePSO;
	}
	


	// Blend States
	// extern ComPtr<ID3D11BlendState> mirrorBS;

	void InitCommonStates(ID3D11Device1* device);

	// ���������� InitCommonStates()���� ���
	void InitSamplers(Microsoft::WRL::ComPtr<ID3D11Device1> device);
	void InitRasterizerStates(ID3D11Device1* device);
	// void InitBlendStates(ComPtr<ID3D11Device>& device);
	// void InitDepthStencilStates(ComPtr<ID3D11Device>& device);
	void InitPipelineStates(Microsoft::WRL::ComPtr<ID3D11Device> device);
	void InitShaders(ID3D11Device1* device);

	// ����: �ʱ�ȭ�� ������ �ʿ��� ��쿡�� �ʱ�ȭ
	// void InitVolumeShaders(ComPtr<ID3D11Device>& device);

	void SetPipelineState(ID3D11DeviceContext1* context, GraphicsPSO& pso);
	void SetPipelineState(ID3D11DeviceContext1* context, ComputePSO& pso);

	HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob);

} // namespace Graphics

