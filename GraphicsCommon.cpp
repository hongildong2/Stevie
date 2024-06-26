#include "pch.h"
#include "GraphicsCommon.h"

#include <d3dcompiler.h>

#pragma comment(lib,"d3dcompiler.lib")

namespace Graphics
{
	Microsoft::WRL::ComPtr<ID3D11SamplerState> linearWrapSS;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> linearClampSS;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> basicRS;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> basicCcwRS;


	Microsoft::WRL::ComPtr<ID3D11VertexShader> basicVS;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> cubemapVS;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> screenQuadVS;


	Microsoft::WRL::ComPtr<ID3D11PixelShader> basicPS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> cubemapPS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> filterCombinePS;


	Microsoft::WRL::ComPtr<ID3D11ComputeShader> downBlurCS;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> upBlurCS;


	Microsoft::WRL::ComPtr<ID3D11InputLayout> basicIL;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> screenQuadIL;

	GraphicsPSO basicPSO;
	GraphicsPSO pbrPSO;
	GraphicsPSO cubemapPSO;
	GraphicsPSO filterCombinePSO;


	ComputePSO downBlurPSO;
	ComputePSO upBlurPSO;



	void SetPipelineState(ID3D11DeviceContext1* context, GraphicsPSO& pso)
	{
		context->VSSetShader(pso.m_vertexShader.Get(), 0, 0);
		context->PSSetShader(pso.m_pixelShader.Get(), 0, 0);
		context->HSSetShader(pso.m_hullShader.Get(), 0, 0);
		context->DSSetShader(pso.m_domainShader.Get(), 0, 0);
		context->GSSetShader(pso.m_geometryShader.Get(), 0, 0);
		context->CSSetShader(NULL, 0, 0);
		context->IASetInputLayout(pso.m_inputLayout.Get());
		context->RSSetState(pso.m_rasterizerState.Get());
		context->OMSetBlendState(pso.m_blendState.Get(), pso.m_blendFactor,
			0xffffffff);
		context->OMSetDepthStencilState(pso.m_depthStencilState.Get(),
			pso.m_stencilRef);
		context->IASetPrimitiveTopology(pso.m_primitiveTopology);
	}

	void SetPipelineState(ID3D11DeviceContext1* context, ComputePSO& pso)
	{
		context->VSSetShader(NULL, 0, 0);
		context->PSSetShader(NULL, 0, 0);
		context->HSSetShader(NULL, 0, 0);
		context->DSSetShader(NULL, 0, 0);
		context->GSSetShader(NULL, 0, 0);
		context->CSSetShader(pso.m_computeShader.Get(), 0, 0);
	}

	// https://learn.microsoft.com/ko-kr/windows/win32/direct3d11/how-to--compile-a-shader
	HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob)
	{
		if (!srcFile || !entryPoint || !profile || !blob)
			return E_INVALIDARG;

		*blob = nullptr;

		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		flags |= D3DCOMPILE_DEBUG;
#endif

		//const D3D_SHADER_MACRO defines[] =
		//{
		//	"EXAMPLE_DEFINE", "1",
		//	NULL, NULL
		//};

		ID3DBlob* shaderBlob = nullptr;
		ID3DBlob* errorBlob = nullptr;
		HRESULT hr = D3DCompileFromFile(srcFile, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entryPoint, profile,
			flags, 0, &shaderBlob, &errorBlob);
		if (FAILED(hr))
		{
			if (errorBlob)
			{
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
				errorBlob->Release();
			}

			if (shaderBlob)
				shaderBlob->Release();

			return hr;
		}

		*blob = shaderBlob;

		return hr;
	}

	void InitShaders(ID3D11Device1* device)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;

		// Vertex Shaders
		{
			HRESULT hr = CompileShader(L"VertexShader.hlsl", "main", "vs_5_0", &shaderBlob);
			DX::ThrowIfFailed(hr);

			D3D11_INPUT_ELEMENT_DESC layout[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
			  D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12,
			  D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20,
			  D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32,
			  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			static_assert((sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC)) == 4, "Basic Vertex Input Layout Size");

			device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, basicVS.GetAddressOf());

			// cubemapVS
			hr = CompileShader(L"CubemapVS.hlsl", "main", "vs_5_0", &shaderBlob);
			DX::ThrowIfFailed(hr);
			device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, cubemapVS.GetAddressOf());
			device->CreateInputLayout(layout, (sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC)), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), basicIL.GetAddressOf());



			D3D11_INPUT_ELEMENT_DESC screenQuadLayout[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
			  D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12,
			  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			static_assert((sizeof(screenQuadLayout) / sizeof(D3D11_INPUT_ELEMENT_DESC)) == 2, "Screen Quad Input Layout Size");

			DX::ThrowIfFailed(CompileShader(L"ScreenQuadVS.hlsl", "main", "vs_5_0", &shaderBlob));
			device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, screenQuadVS.GetAddressOf());
			device->CreateInputLayout(layout, (sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC)), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), screenQuadIL.GetAddressOf());

		}

		// Pixel Shaders
		{
			HRESULT hr = CompileShader(L"PbrPS.hlsl", "main", "ps_5_0", &shaderBlob);

			DX::ThrowIfFailed(hr);
			device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, basicPS.GetAddressOf());

			hr = CompileShader(L"CubemapPS.hlsl", "main", "ps_5_0", &shaderBlob);
			DX::ThrowIfFailed(hr);
			device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, cubemapPS.GetAddressOf());

			DX::ThrowIfFailed(CompileShader(L"FilterCombinePS.hlsl", "main", "ps_5_0", &shaderBlob));
			device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, filterCombinePS.GetAddressOf());
		}

		// Compute Shaders
		{
			DX::ThrowIfFailed(CompileShader(L"downBlurCS.hlsl", "main", "cs_5_0", &shaderBlob));
			device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, downBlurCS.GetAddressOf());

			DX::ThrowIfFailed(CompileShader(L"upBlurCS.hlsl", "main", "cs_5_0", &shaderBlob));
			device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, upBlurCS.GetAddressOf());
		}
	}


	void InitRasterizerStates(ID3D11Device1* device)
	{
		D3D11_RASTERIZER_DESC rastDesc;
		ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC)); // Need this
		rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		// rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
		rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
		rastDesc.FrontCounterClockwise = false;
		rastDesc.DepthClipEnable = true; // <- zNear, zFar 확인에 필요

		device->CreateRasterizerState(&rastDesc, basicRS.GetAddressOf());

		rastDesc.FrontCounterClockwise = true;

		device->CreateRasterizerState(&rastDesc, basicCcwRS.GetAddressOf());
	}

	void InitSamplers(Microsoft::WRL::ComPtr<ID3D11Device1> device)
	{
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;

		HRESULT hr = device->CreateSamplerState(&desc, &linearWrapSS);
		DX::ThrowIfFailed(hr);

		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		hr = device->CreateSamplerState(&desc, &linearClampSS);

		DX::ThrowIfFailed(hr);

	}
	void InitPipelineStates(Microsoft::WRL::ComPtr<ID3D11Device> device)
	{
		basicPSO.m_vertexShader = basicVS;
		basicPSO.m_inputLayout = basicIL;
		basicPSO.m_pixelShader = basicPS;
		basicPSO.m_rasterizerState = basicRS;
		basicPSO.m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		cubemapPSO.m_vertexShader = cubemapVS;
		cubemapPSO.m_inputLayout = basicIL;
		cubemapPSO.m_pixelShader = cubemapPS;
		cubemapPSO.m_rasterizerState = basicCcwRS;
		cubemapPSO.m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		filterCombinePSO.m_vertexShader = screenQuadVS;
		filterCombinePSO.m_inputLayout = screenQuadIL;
		filterCombinePSO.m_pixelShader = filterCombinePS;
		filterCombinePSO.m_rasterizerState = basicRS;
		filterCombinePSO.m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


		upBlurPSO.m_computeShader = upBlurCS;
		downBlurPSO.m_computeShader = downBlurCS;
	}

	void InitCommonStates(ID3D11Device1* device)
	{
		InitSamplers(device);

		InitShaders(device);

		InitRasterizerStates(device);

		InitPipelineStates(device);
	}



}