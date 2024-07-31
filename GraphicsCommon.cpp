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

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> basicRS;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> basicCcwRS;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> wireframeCwRS;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> wireframeCcwRS;



	Microsoft::WRL::ComPtr<ID3D11VertexShader> basicVS;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> cubemapVS;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> screenQuadVS;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> depthOnlyVS;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> shadowMapVS;



	Microsoft::WRL::ComPtr<ID3D11PixelShader> basicPS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> cubemapPS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> filterCombinePS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> fogPS;

	Microsoft::WRL::ComPtr<ID3D11ComputeShader> downBlurCS;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> upBlurCS;


	Microsoft::WRL::ComPtr<ID3D11HullShader> tessellatedQuadHS;
	Microsoft::WRL::ComPtr<ID3D11HullShader> tessellatedQuadDepthOnlyHS;
	Microsoft::WRL::ComPtr<ID3D11DomainShader> tessellatedQuadDS;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> basicIL;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> samplingIL;

	GraphicsPSO basicPSO;
	GraphicsPSO pbrPSO;
	GraphicsPSO cubemapPSO;
	GraphicsPSO filterCombinePSO;

	GraphicsPSO depthOnlyPSO;
	GraphicsPSO cubeMapDepthOnlyPSO;
	GraphicsPSO fogPSO;

	GraphicsPSO cubeMapShadowMapPSO;
	GraphicsPSO shadowMapPSO;


	ComputePSO downBlurPSO;
	ComputePSO upBlurPSO;

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
		GraphicsPSO shadowMapPSO;

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
		Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
		D3D_SHADER_MACRO depthOnlyShaderDefines[2] = { "DEPTH_ONLY", "1", NULL, NULL };
		D3D_SHADER_MACRO oceanShaderDefines[2] = { "OCEAN_SHADER", "1", NULL, NULL };
		D3D_SHADER_MACRO skyBoxShaderDefines[2] = { "SKY_BOX", "1", NULL, NULL };
		D3D_SHADER_MACRO shadowMapSD[2] = { "SHADOW_MAP", "1", NULL, NULL };

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

			DX::ThrowIfFailed(CompileShader(L"PBRVS.hlsl", "main", "vs_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, basicVS.GetAddressOf());

			// IL만드는데 왜 굳이 쉐이더 바이너리를 넣어줘야할까? 왜 한번만 하면 동일한 레이아웃의 다른 쉐이더들은 안해줘도 될까?
			device->CreateInputLayout(layout, (sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC)), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), basicIL.GetAddressOf());

			DX::ThrowIfFailed(CompileShader(L"SamplingVS.hlsl", "main", "vs_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, cubemapVS.GetAddressOf());

			DX::ThrowIfFailed(CompileShader(L"SamplingVS.hlsl", "main", "vs_5_0", depthOnlyShaderDefines, shaderBlob.GetAddressOf()));
			device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, depthOnlyVS.GetAddressOf());

			DX::ThrowIfFailed(CompileShader(L"SamplingVS.hlsl", "main", "vs_5_0", depthOnlyShaderDefines, shaderBlob.GetAddressOf()));
			device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, depthOnlyVS.GetAddressOf());

			DX::ThrowIfFailed(CompileShader(L"SamplingVS.hlsl", "main", "vs_5_0", shadowMapSD, shaderBlob.GetAddressOf()));
			device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, shadowMapVS.GetAddressOf());



			D3D11_INPUT_ELEMENT_DESC screenQuadLayout[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
			  D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12,
			  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			static_assert((sizeof(screenQuadLayout) / sizeof(D3D11_INPUT_ELEMENT_DESC)) == 2, "Screen Quad Input Layout Size");

			DX::ThrowIfFailed(CompileShader(L"ScreenQuadVS.hlsl", "main", "vs_5_0", NULL, shaderBlob.GetAddressOf()));
			DX::ThrowIfFailed(device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, screenQuadVS.GetAddressOf()));

			DX::ThrowIfFailed(device->CreateInputLayout(layout, (sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC)), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), samplingIL.GetAddressOf()));




		}

		// Pixel Shaders
		{
			DX::ThrowIfFailed(CompileShader(L"PBRPS.hlsl", "main", "ps_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, basicPS.GetAddressOf());

			// oceanPS
			//DX::ThrowIfFailed(CompileShader(L"PBRPS.hlsl", "main", "ps_5_0", oceanShaderDefines, shaderBlob.GetAddressOf()));
			//device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, Ocean::oceanPS.GetAddressOf());

			DX::ThrowIfFailed(CompileShader(L"OceanSurfacePS.hlsl", "main", "ps_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, Ocean::oceanPS.GetAddressOf());


			DX::ThrowIfFailed(CompileShader(L"skyboxPS.hlsl", "main", "ps_5_0", skyBoxShaderDefines, shaderBlob.GetAddressOf()));
			device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, cubemapPS.GetAddressOf());

			DX::ThrowIfFailed(CompileShader(L"FilterCombinePS.hlsl", "main", "ps_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, filterCombinePS.GetAddressOf());

			DX::ThrowIfFailed(CompileShader(L"FogPS.hlsl", "main", "ps_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, fogPS.GetAddressOf());

		}

		// Compute Shaders
		{
			DX::ThrowIfFailed(CompileShader(L"downBlurCS.hlsl", "main", "cs_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, downBlurCS.GetAddressOf());

			DX::ThrowIfFailed(CompileShader(L"upBlurCS.hlsl", "main", "cs_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, upBlurCS.GetAddressOf());

			DX::ThrowIfFailed(CompileShader(L"initialOceanSpectrumCS.hlsl", "main", "cs_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, Ocean::initialSpectrumCS.GetAddressOf());

			DX::ThrowIfFailed(CompileShader(L"TimeDependentWaveSpectrumCS.hlsl", "main", "cs_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, Ocean::timedependentSpectrumCS.GetAddressOf());

			DX::ThrowIfFailed(CompileShader(L"OceanFFTCS.hlsl", "main", "cs_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, Ocean::FFTCS.GetAddressOf());

			DX::ThrowIfFailed(CompileShader(L"OceanFFTCS.hlsl", "PostProcess", "cs_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, Ocean::FFTPostProcessCS.GetAddressOf());

			DX::ThrowIfFailed(CompileShader(L"CombineWaveCS.hlsl", "main", "cs_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, Ocean::combineWaveCS.GetAddressOf());

			DX::ThrowIfFailed(CompileShader(L"FoamSimulationCS.hlsl", "main", "cs_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, Ocean::foamSimulationCS.GetAddressOf());
		}

		// Hull, Domain Shaders
		{
			DX::ThrowIfFailed(CompileShader(L"TessellatedQuadHS.hlsl", "main", "hs_5_0", NULL, shaderBlob.GetAddressOf()));
			device->CreateHullShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, tessellatedQuadHS.GetAddressOf());

			DX::ThrowIfFailed(CompileShader(L"TessellatedQuadHS.hlsl", "main", "hs_5_0", depthOnlyShaderDefines, shaderBlob.GetAddressOf()));
			device->CreateHullShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, tessellatedQuadDepthOnlyHS.GetAddressOf());

			DX::ThrowIfFailed(CompileShader(L"TessellatedQuadDS.hlsl", "main", "ds_5_0", NULL, shaderBlob.GetAddressOf()));
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
		depthOnlyPSO.m_inputLayout = basicIL;
		depthOnlyPSO.m_rasterizerState = basicRS;
		depthOnlyPSO.m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		shadowMapPSO = depthOnlyPSO;
		shadowMapPSO.m_vertexShader = shadowMapVS;

		cubeMapDepthOnlyPSO = depthOnlyPSO;
		cubeMapDepthOnlyPSO.m_rasterizerState = basicCcwRS;

		cubeMapShadowMapPSO = shadowMapPSO;
		cubeMapShadowMapPSO.m_rasterizerState = basicCcwRS;

		fogPSO.m_vertexShader = screenQuadVS;
		fogPSO.m_inputLayout = basicIL;
		fogPSO.m_pixelShader = fogPS;
		fogPSO.m_rasterizerState = basicRS;
		fogPSO.m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		upBlurPSO.m_computeShader = upBlurCS;
		downBlurPSO.m_computeShader = downBlurCS;

		Ocean::initialSpectrumPSO.m_computeShader = Ocean::initialSpectrumCS;
		Ocean::timedependentSpectrumPSO.m_computeShader = Ocean::timedependentSpectrumCS;
		Ocean::FFTPSO.m_computeShader = Ocean::FFTCS;
		Ocean::FFTPostProcessPSO.m_computeShader = Ocean::FFTPostProcessCS;
		Ocean::combineWavePSO.m_computeShader = Ocean::combineWaveCS;
		Ocean::foamSimulationPSO.m_computeShader = Ocean::foamSimulationCS;

		Ocean::depthOnlyPSO = depthOnlyPSO;
		Ocean::depthOnlyPSO.m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
		Ocean::depthOnlyPSO.m_hullShader = tessellatedQuadDepthOnlyHS;
		Ocean::depthOnlyPSO.m_domainShader = tessellatedQuadDS;

		Ocean::shadowMapPSO = Ocean::depthOnlyPSO;
		Ocean::shadowMapPSO.m_vertexShader = shadowMapVS;

		Ocean::oceanPSO.m_vertexShader = basicVS;
		Ocean::oceanPSO.m_inputLayout = basicIL;
		Ocean::oceanPSO.m_pixelShader = Ocean::oceanPS;
		Ocean::oceanPSO.m_rasterizerState = basicRS;
		Ocean::oceanPSO.m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
		Ocean::oceanPSO.m_hullShader = tessellatedQuadHS;
		Ocean::oceanPSO.m_domainShader = tessellatedQuadDS;
	}

	void InitCommonStates(ID3D11Device1* device)
	{
		InitSamplers(device);

		InitShaders(device);

		InitRasterizerStates(device);

		InitPipelineStates(device);
	}



}