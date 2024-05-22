#pragma once

// 참고: DirectX_Graphic-Samples 미니엔진
// https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Core/GraphicsCommon.h

namespace Graphics
{

	// Samplers
	extern Microsoft::WRL::ComPtr<ID3D11SamplerState> linearWrapSS;
	extern Microsoft::WRL::ComPtr<ID3D11SamplerState> linearClampSS;
	// extern ComPtr<ID3D11SamplerState> pointClampSS;
	// extern ComPtr<ID3D11SamplerState> pointWrapSS;

	extern Microsoft::WRL::ComPtr<ID3D11RasterizerState> basicRS;

	// Depth Stencil States
	// extern ComPtr<ID3D11DepthStencilState> basicDSS; // 일반적으로 그리기


	// Shaders
	extern Microsoft::WRL::ComPtr<ID3D11VertexShader> basicVS;

	extern Microsoft::WRL::ComPtr<ID3D11PixelShader> basicPS;

	// Input Layouts
	extern Microsoft::WRL::ComPtr<ID3D11InputLayout> basicIL;

	// Blend States
	// extern ComPtr<ID3D11BlendState> mirrorBS;

	void InitCommonStates(ID3D11Device1* device);

	// 내부적으로 InitCommonStates()에서 사용
	void InitSamplers(Microsoft::WRL::ComPtr<ID3D11Device1> device);
	void InitRasterizerStates(ID3D11Device1* device);
	// void InitBlendStates(ComPtr<ID3D11Device>& device);
	// void InitDepthStencilStates(ComPtr<ID3D11Device>& device);
	// void InitPipelineStates(ComPtr<ID3D11Device>& device);
	void InitShaders(ID3D11Device1* device);

	// 주의: 초기화가 느려서 필요한 경우에만 초기화
	// void InitVolumeShaders(ComPtr<ID3D11Device>& device);

	HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob);

} // namespace Graphics

