#pragma once

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

	// Depth Stencil States
	// extern ComPtr<ID3D11DepthStencilState> basicDSS; // �Ϲ������� �׸���


	// Shaders
	extern Microsoft::WRL::ComPtr<ID3D11VertexShader> basicVS;

	extern Microsoft::WRL::ComPtr<ID3D11PixelShader> basicPS;

	// Input Layouts
	extern Microsoft::WRL::ComPtr<ID3D11InputLayout> basicIL;

	// Blend States
	// extern ComPtr<ID3D11BlendState> mirrorBS;

	void InitCommonStates(ID3D11Device1* device);

	// ���������� InitCommonStates()���� ���
	void InitSamplers(Microsoft::WRL::ComPtr<ID3D11Device1> device);
	void InitRasterizerStates(ID3D11Device1* device);
	// void InitBlendStates(ComPtr<ID3D11Device>& device);
	// void InitDepthStencilStates(ComPtr<ID3D11Device>& device);
	// void InitPipelineStates(ComPtr<ID3D11Device>& device);
	void InitShaders(ID3D11Device1* device);

	// ����: �ʱ�ȭ�� ������ �ʿ��� ��쿡�� �ʱ�ȭ
	// void InitVolumeShaders(ComPtr<ID3D11Device>& device);

	HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob);

} // namespace Graphics

