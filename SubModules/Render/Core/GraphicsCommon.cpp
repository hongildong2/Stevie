#include "pch.h"
#include "pch.h"
#include "GraphicsCommon.h"
#include "Utility.h"
#include <d3dcompiler.h>

#pragma comment(lib,"d3dcompiler.lib")

namespace Graphics
{
	Microsoft::WRL::ComPtr<ID3D11SamplerState> linearWrapSS;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> linearClampSS;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> linearMirrorSS;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowPointSS;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowCompareSS;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> basicRS;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> basicCcwRS;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> wireframeCwRS;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> wireframeCcwRS;



	Microsoft::WRL::ComPtr<ID3D11VertexShader> basicVS;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> cubemapVS;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> screenQuadVS;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> depthOnlyVS;



	Microsoft::WRL::ComPtr<ID3D11PixelShader> basicPS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> cubemapPS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> filterCombinePS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> fogPS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> volumePS;

	Microsoft::WRL::ComPtr<ID3D11ComputeShader> downBlurCS;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> upBlurCS;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> cloudDensityCS;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> cloudLightingCS;


	Microsoft::WRL::ComPtr<ID3D11HullShader> tessellatedQuadHS;
	Microsoft::WRL::ComPtr<ID3D11HullShader> tessellatedQuadDepthOnlyHS;
	Microsoft::WRL::ComPtr<ID3D11DomainShader> tessellatedQuadDS;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> basicIL;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> samplingIL;

	Microsoft::WRL::ComPtr<ID3D11BlendState> alphaBS;

	GraphicsPSO basicPSO;
	GraphicsPSO pbrPSO;
	GraphicsPSO cubemapPSO;
	GraphicsPSO filterCombinePSO;

	GraphicsPSO depthOnlyPSO;
	GraphicsPSO cubeMapDepthOnlyPSO;
	GraphicsPSO fogPSO;
	GraphicsPSO cloudPSO;

	ComputePSO downBlurPSO;
	ComputePSO upBlurPSO;
	ComputePSO cloudDensityPSO;
	ComputePSO cloudLightingPSO;

	// 이게 맞는걸까??
	namespace Ocean
	{
		Microsoft::WRL::ComPtr<ID3D11ComputeShader> initialSpectrumCS;
		Microsoft::WRL::ComPtr<ID3D11ComputeShader> timedependentSpectrumCS;
		Microsoft::WRL::ComPtr<ID3D11ComputeShader> FFTCS;
		Microsoft::WRL::ComPtr<ID3D11ComputeShader> FFTPostProcessCS;
		Microsoft::WRL::ComPtr<ID3D11ComputeShader> combineWaveCS;
		Microsoft::WRL::ComPtr<ID3D11ComputeShader> foamSimulationCS;

		Microsoft::WRL::ComPtr<ID3D11PixelShader> oceanPS;

		ComputePSO initialSpectrumPSO;
		ComputePSO timedependentSpectrumPSO;
		ComputePSO FFTPSO;
		ComputePSO FFTPostProcessPSO;
		ComputePSO combineWavePSO;
		ComputePSO foamSimulationPSO;

		GraphicsPSO oceanPSO;
		GraphicsPSO depthOnlyPSO;
	}



	void SetPipelineState(ID3D11DeviceContext1* context, const GraphicsPSO& pso)
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

	void SetPipelineState(ID3D11DeviceContext1* context, const ComputePSO& pso)
	{
		context->VSSetShader(NULL, 0, 0);
		context->PSSetShader(NULL, 0, 0);
		context->HSSetShader(NULL, 0, 0);
		context->DSSetShader(NULL, 0, 0);
		context->GSSetShader(NULL, 0, 0);
		context->CSSetShader(pso.m_computeShader.Get(), 0, 0);
	}

	// https://learn.microsoft.com/ko-kr/windows/win32/direct3d11/how-to--compile-a-shader
	HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, const D3D_SHADER_MACRO* defines, _Outptr_ ID3DBlob** blob)
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
		HRESULT hr = D3DCompileFromFile(srcFile, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
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
		// TODO :: File... System... 만들..기....잉..
		const wchar_t* BASE_PATH = L"SubModules/Render/Shaders/";
		constexpr size_t BUFFER_COUNT = 500;
		wchar_t pathBuffer[BUFFER_COUNT] = { NULL, };

		Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
		D3D_SHADER_MACRO depthOnlyShaderDefines[2] = { "DEPTH_ONLY", "1", NULL, NULL };
		D3D_SHADER_MACRO oceanShaderDefines[2] = { "OCEAN_SHADER", "1", NULL, NULL };
		D3D_SHADER_MACRO skyBoxShaderDefines[2] = { "SKY_BOX", "1", NULL, NULL };
		D3D_SHADER_MACRO shadowMapSD[2] = { "SHADOW_MAP", "1", NULL, NULL };
		D3D_SHADER_MACRO cloudDensityDefines[2] = { "DENSITY", "1", NULL, NULL };

		// Vertex Shaders
		{
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


			assert(&shaderBlob != nullptr);

			swprintf(pathBuffer, BUFFER_COUNT, L"%s%s", BASE_PATH, L"PBRVS.hlsl");
			DX::ThrowIfFailed(CompileShader(pathBuffer, "main", "vs_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, basicVS.GetAddressOf());

			// IL만드는데 왜 굳이 쉐이더 바이너리를 넣어줘야할까? 왜 한번만 하면 동일한 레이아웃의 다른 쉐이더들은 안해줘도 될까?
			device->CreateInputLayout(layout, (sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC)), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), basicIL.GetAddressOf());


			swprintf(pathBuffer, BUFFER_COUNT, L"%s%s", BASE_PATH, L"SamplingVS.hlsl");
			DX::ThrowIfFailed(CompileShader(pathBuffer, "main", "vs_5_0", skyBoxShaderDefines, shaderBlob.GetAddressOf()));
			device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, cubemapVS.GetAddressOf());

			swprintf(pathBuffer, BUFFER_COUNT, L"%s%s", BASE_PATH, L"SamplingVS.hlsl");
			DX::ThrowIfFailed(CompileShader(pathBuffer, "main", "vs_5_0", depthOnlyShaderDefines, shaderBlob.GetAddressOf()));
			device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, depthOnlyVS.GetAddressOf());



			D3D11_INPUT_ELEMENT_DESC screenQuadLayout[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
			  D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12,
			  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			static_assert((sizeof(screenQuadLayout) / sizeof(D3D11_INPUT_ELEMENT_DESC)) == 2, "Screen Quad Input Layout Size");

			swprintf(pathBuffer, BUFFER_COUNT, L"%s%s", BASE_PATH, L"ScreenQuadVS.hlsl");
			DX::ThrowIfFailed(CompileShader(pathBuffer, "main", "vs_5_0", NULL, shaderBlob.GetAddressOf()));
			DX::ThrowIfFailed(device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, screenQuadVS.GetAddressOf()));

			DX::ThrowIfFailed(device->CreateInputLayout(layout, (sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC)), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), samplingIL.GetAddressOf()));


		}

		// Pixel Shaders
		{
			swprintf(pathBuffer, BUFFER_COUNT, L"%s%s", BASE_PATH, L"PBRPS.hlsl");
			DX::ThrowIfFailed(CompileShader(pathBuffer, "main", "ps_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, basicPS.GetAddressOf());

			swprintf(pathBuffer, BUFFER_COUNT, L"%s%s", BASE_PATH, L"OceanSurfacePS.hlsl");
			DX::ThrowIfFailed(CompileShader(pathBuffer, "main", "ps_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, Ocean::oceanPS.GetAddressOf());

			swprintf(pathBuffer, BUFFER_COUNT, L"%s%s", BASE_PATH, L"skyboxPS.hlsl");
			DX::ThrowIfFailed(CompileShader(pathBuffer, "main", "ps_5_0", skyBoxShaderDefines, shaderBlob.GetAddressOf()));
			device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, cubemapPS.GetAddressOf());

			swprintf(pathBuffer, BUFFER_COUNT, L"%s%s", BASE_PATH, L"FilterCombinePS.hlsl");
			DX::ThrowIfFailed(CompileShader(pathBuffer, "main", "ps_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, filterCombinePS.GetAddressOf());

			swprintf(pathBuffer, BUFFER_COUNT, L"%s%s", BASE_PATH, L"FogPS.hlsl");
			DX::ThrowIfFailed(CompileShader(pathBuffer, "main", "ps_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, fogPS.GetAddressOf());

			swprintf(pathBuffer, BUFFER_COUNT, L"%s%s", BASE_PATH, L"VolumePS.hlsl");
			DX::ThrowIfFailed(CompileShader(pathBuffer, "main", "ps_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, volumePS.GetAddressOf());

		}

		// Compute Shaders
		{
			swprintf(pathBuffer, BUFFER_COUNT, L"%s%s", BASE_PATH, L"downBlurCS.hlsl");
			DX::ThrowIfFailed(CompileShader(pathBuffer, "main", "cs_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, downBlurCS.GetAddressOf());

			swprintf(pathBuffer, BUFFER_COUNT, L"%s%s", BASE_PATH, L"upBlurCS.hlsl");
			DX::ThrowIfFailed(CompileShader(pathBuffer, "main", "cs_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, upBlurCS.GetAddressOf());

			swprintf(pathBuffer, BUFFER_COUNT, L"%s%s", BASE_PATH, L"initialOceanSpectrumCS.hlsl");
			DX::ThrowIfFailed(CompileShader(pathBuffer, "main", "cs_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, Ocean::initialSpectrumCS.GetAddressOf());

			swprintf(pathBuffer, BUFFER_COUNT, L"%s%s", BASE_PATH, L"TimeDependentWaveSpectrumCS.hlsl");
			DX::ThrowIfFailed(CompileShader(pathBuffer, "main", "cs_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, Ocean::timedependentSpectrumCS.GetAddressOf());

			swprintf(pathBuffer, BUFFER_COUNT, L"%s%s", BASE_PATH, L"OceanFFTCS.hlsl");
			DX::ThrowIfFailed(CompileShader(pathBuffer, "main", "cs_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, Ocean::FFTCS.GetAddressOf());

			swprintf(pathBuffer, BUFFER_COUNT, L"%s%s", BASE_PATH, L"OceanFFTCS.hlsl");
			DX::ThrowIfFailed(CompileShader(pathBuffer, "PostProcess", "cs_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, Ocean::FFTPostProcessCS.GetAddressOf());

			swprintf(pathBuffer, BUFFER_COUNT, L"%s%s", BASE_PATH, L"CombineWaveCS.hlsl");
			DX::ThrowIfFailed(CompileShader(pathBuffer, "main", "cs_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, Ocean::combineWaveCS.GetAddressOf());

			swprintf(pathBuffer, BUFFER_COUNT, L"%s%s", BASE_PATH, L"FoamSimulationCS.hlsl");
			DX::ThrowIfFailed(CompileShader(pathBuffer, "main", "cs_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, Ocean::foamSimulationCS.GetAddressOf());

			swprintf(pathBuffer, BUFFER_COUNT, L"%s%s", BASE_PATH, L"CloudCS.hlsl");
			DX::ThrowIfFailed(CompileShader(pathBuffer, "CloudDensity", "cs_5_0", cloudDensityDefines, shaderBlob.GetAddressOf()));
			device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, cloudDensityCS.GetAddressOf());

			swprintf(pathBuffer, BUFFER_COUNT, L"%s%s", BASE_PATH, L"CloudCS.hlsl");
			DX::ThrowIfFailed(CompileShader(pathBuffer, "CloudLighting", "cs_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, cloudLightingCS.GetAddressOf());
		}

		// Hull, Domain Shaders
		{
			swprintf(pathBuffer, BUFFER_COUNT, L"%s%s", BASE_PATH, L"TessellatedQuadHS.hlsl");
			DX::ThrowIfFailed(CompileShader(pathBuffer, "main", "hs_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreateHullShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, tessellatedQuadHS.GetAddressOf());

			swprintf(pathBuffer, BUFFER_COUNT, L"%s%s", BASE_PATH, L"TessellatedQuadHS.hlsl");
			DX::ThrowIfFailed(CompileShader(pathBuffer, "main", "hs_5_0", depthOnlyShaderDefines, shaderBlob.GetAddressOf()));
			device->CreateHullShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, tessellatedQuadDepthOnlyHS.GetAddressOf());

			swprintf(pathBuffer, BUFFER_COUNT, L"%s%s", BASE_PATH, L"TessellatedQuadDS.hlsl");
			DX::ThrowIfFailed(CompileShader(pathBuffer, "main", "ds_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreateDomainShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, tessellatedQuadDS.GetAddressOf());


		}
	}


	void InitRasterizerStates(ID3D11Device1* device)
	{
		D3D11_RASTERIZER_DESC rastDesc;
		ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC)); // Need this
		rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;

		rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
		rastDesc.FrontCounterClockwise = false;
		rastDesc.DepthClipEnable = true; // <- zNear, zFar 확인에 필요

		DX::ThrowIfFailed(device->CreateRasterizerState(&rastDesc, basicRS.GetAddressOf()));

		rastDesc.FrontCounterClockwise = true;

		DX::ThrowIfFailed(device->CreateRasterizerState(&rastDesc, basicCcwRS.GetAddressOf()));

		rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
		DX::ThrowIfFailed(device->CreateRasterizerState(&rastDesc, wireframeCcwRS.GetAddressOf()));

		rastDesc.FrontCounterClockwise = false;
		DX::ThrowIfFailed(device->CreateRasterizerState(&rastDesc, wireframeCwRS.GetAddressOf()));
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

		HRESULT hr = device->CreateSamplerState(&desc, linearWrapSS.GetAddressOf());
		DX::ThrowIfFailed(hr);

		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		hr = device->CreateSamplerState(&desc, linearClampSS.GetAddressOf());

		DX::ThrowIfFailed(hr);


		desc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;

		DX::ThrowIfFailed(device->CreateSamplerState(&desc, linearMirrorSS.GetAddressOf()));

		desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.BorderColor[0] = 300.0f; // 큰 Z값
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		DX::ThrowIfFailed(device->CreateSamplerState(&desc, shadowPointSS.GetAddressOf()));

		desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.BorderColor[0] = 300.0f; // 큰 Z값
		desc.Filter =
			D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		DX::ThrowIfFailed(device->CreateSamplerState(&desc, shadowCompareSS.GetAddressOf()));
	}

	void InitBlendStates(Microsoft::WRL::ComPtr<ID3D11Device> device)
	{

		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(blendDesc));
		blendDesc.AlphaToCoverageEnable = false; // <- 주의: FALSE
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask =
			D3D11_COLOR_WRITE_ENABLE_ALL;
		DX::ThrowIfFailed(device->CreateBlendState(&blendDesc, alphaBS.GetAddressOf()));
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
		filterCombinePSO.m_inputLayout = samplingIL;
		filterCombinePSO.m_pixelShader = filterCombinePS;
		filterCombinePSO.m_rasterizerState = basicRS;
		filterCombinePSO.m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		depthOnlyPSO.m_vertexShader = depthOnlyVS;		// depthOnlyPSO.m_pixelShader = 설정안해줘도 뎁스버퍼 생김
		depthOnlyPSO.m_inputLayout = samplingIL;
		depthOnlyPSO.m_rasterizerState = basicRS;
		depthOnlyPSO.m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		cubeMapDepthOnlyPSO = depthOnlyPSO;
		cubeMapDepthOnlyPSO.m_rasterizerState = basicCcwRS;

		fogPSO.m_vertexShader = screenQuadVS;
		fogPSO.m_inputLayout = basicIL;
		fogPSO.m_pixelShader = fogPS;
		fogPSO.m_rasterizerState = basicRS;
		fogPSO.m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		cloudPSO = basicPSO;
		cloudPSO.m_pixelShader = volumePS;
		cloudPSO.m_blendState = alphaBS;

		cloudDensityPSO.m_computeShader = cloudDensityCS;
		cloudLightingPSO.m_computeShader = cloudLightingCS;

		upBlurPSO.m_computeShader = upBlurCS;
		downBlurPSO.m_computeShader = downBlurCS;

		Ocean::initialSpectrumPSO.m_computeShader = Ocean::initialSpectrumCS;
		Ocean::timedependentSpectrumPSO.m_computeShader = Ocean::timedependentSpectrumCS;
		Ocean::FFTPSO.m_computeShader = Ocean::FFTCS;
		Ocean::FFTPostProcessPSO.m_computeShader = Ocean::FFTPostProcessCS;
		Ocean::combineWavePSO.m_computeShader = Ocean::combineWaveCS;
		Ocean::foamSimulationPSO.m_computeShader = Ocean::foamSimulationCS;

		// 수정해라
		Ocean::depthOnlyPSO = depthOnlyPSO;
		Ocean::depthOnlyPSO.m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
		Ocean::depthOnlyPSO.m_hullShader = tessellatedQuadDepthOnlyHS;
		Ocean::depthOnlyPSO.m_domainShader = tessellatedQuadDS;

		Ocean::oceanPSO.m_vertexShader = basicVS;
		Ocean::oceanPSO.m_inputLayout = basicIL;
		Ocean::oceanPSO.m_pixelShader = Ocean::oceanPS;
		Ocean::oceanPSO.m_rasterizerState = basicRS;
		Ocean::oceanPSO.m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
		Ocean::oceanPSO.m_hullShader = tessellatedQuadHS;
		Ocean::oceanPSO.m_domainShader = tessellatedQuadDS;
		Ocean::oceanPSO.m_blendState = alphaBS;
	}

	void InitCommonStates(ID3D11Device1* device)
	{
		InitSamplers(device);

		InitShaders(device);

		InitRasterizerStates(device);

		InitBlendStates(device);

		InitPipelineStates(device);
	}



}