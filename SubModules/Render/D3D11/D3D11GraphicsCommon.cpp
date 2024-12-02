#include "pch.h"

#include "D3D11Renderer.h"
#include "D3D11Resources.h"
#include "D3D11DeviceResources.h"
#include "D3DUtil.h"

namespace Graphics
{
	RVertexShader* BASIC_VS;
	RVertexShader* CUBEMAP_VS;
	RVertexShader* QUAD_VS;
	RVertexShader* DEPTH_ONLY_VS;
	RVertexShader* SAMPLING_VS;


	RPixelShader* BASIC_PS;
	RPixelShader* CUBEMAP_PS;
	RPixelShader* FILTER_COMBINE_PS;
	RPixelShader* FOG_PS;
	RPixelShader* VOLUME_PS;
	RPixelShader* OCEAN_SURFACE_PS;
	RPixelShader* DEMO_PS;


	RComputeShader* DOWN_BLUR_CS;
	RComputeShader* UP_BLUR_CS;
	RComputeShader* CLOUD_DENSITY_CS;
	RComputeShader* CLOUD_LIGHTING_CS;
	RComputeShader* OCEAN_INITIAL_SPECTRUM_CS;
	RComputeShader* OCEAN_TIME_DEPENDENT_SPECTRUM_CS;
	RComputeShader* OCEAN_FFT_CS;
	RComputeShader* OCEAN_FFT_POST_PROCESS_CS;
	RComputeShader* OCEAN_COMBINE_WAVE_CS;
	RComputeShader* OCEAN_FOAM_SIMULATION_CS;


	RHullShader* TESSELLATED_QUAD_HS;

	RDomainShader* TESSELATED_QUAD_DS;


	// Samplers
	RSamplerState* LINEAR_WRAP_SS;
	RSamplerState* LINEAR_CLAMP_SS;
	RSamplerState* LINEAR_MIRROR_SS;
	RSamplerState* SHADOW_POINT_SS;
	RSamplerState* SHADOW_COMPARE_SS;

	RRasterizerState* SOLID_CW_RS;
	RRasterizerState* SOLID_CCW_RS;
	RRasterizerState* WIRE_FRAME_CW_RS;
	RRasterizerState* WIRE_FRAME_CCW_RS;


	// Input Layouts
	RInputLayout* BASIC_IL;
	RInputLayout* SAMPLING_IL;

	RBlendState* ALPHA_BS;

	namespace D3D11
	{
		void CheckValidity()
		{

		}
		void InitCommonResources(const D3D11Renderer* pRenderer)
		{
			InitShaders(pRenderer);
			InitSamplerStates(pRenderer);
			InitBlendStates(pRenderer);
			InitRasterizerStates(pRenderer);
			InitDepthStencilStates(pRenderer);
		}
		void InitShaders(const D3D11Renderer* pRenderer)
		{
			const WCHAR* BASE_PATH = pRenderer->GetShaderPath();
			auto* pDevice = pRenderer->GetDeviceResources()->GetD3DDevice();
			constexpr size_t BUFFER_COUNT = 500;
			WCHAR shaderFileNameBuffer[BUFFER_COUNT] = { NULL, };

			Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;



			// VERTEX SHADER
			{
				auto* lBasicVS = new D3D11VertexShader(L"BASIC");
				auto* lCubemapVS = new D3D11VertexShader(L"CUBEMAP");
				auto* lQuadVS = new D3D11VertexShader(L"QUAD");
				auto* lDepthOnlyVS = new D3D11VertexShader(L"DEPTH_ONLY");
				auto* lSamplingVS = new D3D11VertexShader(L"SAMPLING");


				// Input Layouts
				RInputLayout* lBasicIL = new RInputLayout();
				RInputLayout* lSamplingIL = new RInputLayout();


				swprintf(shaderFileNameBuffer, BUFFER_COUNT, L"%s%s_%s.hlsl", BASE_PATH, lBasicVS->GetName(), ToString(EShaderType::VERTEX_SHADER));
				ThrowIfFailed(CompileShader(shaderFileNameBuffer, "main", "vs_5_0", NULL, shaderBlob.GetAddressOf()));
				ThrowIfFailed(pDevice->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, lBasicVS->ReleaseAndGetAddressOf()));
				BASIC_VS = lBasicVS;

				const D3D11_INPUT_ELEMENT_DESC BASIC_INPUT_LAYOUT[] =
				{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				};
				ThrowIfFailed(pDevice->CreateInputLayout(BASIC_INPUT_LAYOUT, (sizeof(BASIC_INPUT_LAYOUT) / sizeof(D3D11_INPUT_ELEMENT_DESC)), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), lBasicIL->ReleaseAndGetAddressOf()));
				BASIC_IL = lBasicIL;



				swprintf(shaderFileNameBuffer, BUFFER_COUNT, L"%s%s_%s.hlsl", BASE_PATH, lQuadVS->GetName(), ToString(EShaderType::VERTEX_SHADER));
				ThrowIfFailed(CompileShader(shaderFileNameBuffer, "main", "vs_5_0", NULL, shaderBlob.GetAddressOf()));
				ThrowIfFailed(pDevice->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, lQuadVS->ReleaseAndGetAddressOf()));
				QUAD_VS = lQuadVS;


				// SAMPLING INPUTS
				swprintf(shaderFileNameBuffer, BUFFER_COUNT, L"%s%s_%s.hlsl", BASE_PATH, lCubemapVS->GetName(), ToString(EShaderType::VERTEX_SHADER));
				ThrowIfFailed(CompileShader(shaderFileNameBuffer, "main", "vs_5_0", NULL, shaderBlob.GetAddressOf()));
				ThrowIfFailed(pDevice->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, lCubemapVS->ReleaseAndGetAddressOf()));
				CUBEMAP_VS = lCubemapVS;

				swprintf(shaderFileNameBuffer, BUFFER_COUNT, L"%s%s_%s.hlsl", BASE_PATH, lDepthOnlyVS->GetName(), ToString(EShaderType::VERTEX_SHADER));
				ThrowIfFailed(CompileShader(shaderFileNameBuffer, "main", "vs_5_0", NULL, shaderBlob.GetAddressOf()));
				ThrowIfFailed(pDevice->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, lDepthOnlyVS->ReleaseAndGetAddressOf()));
				DEPTH_ONLY_VS = lDepthOnlyVS;

				swprintf(shaderFileNameBuffer, BUFFER_COUNT, L"%s%s_%s.hlsl", BASE_PATH, lSamplingVS->GetName(), ToString(EShaderType::VERTEX_SHADER));
				ThrowIfFailed(CompileShader(shaderFileNameBuffer, "main", "vs_5_0", NULL, shaderBlob.GetAddressOf()));
				ThrowIfFailed(pDevice->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, lSamplingVS->ReleaseAndGetAddressOf()));
				SAMPLING_VS = lSamplingVS;


				const D3D11_INPUT_ELEMENT_DESC SAMPLING_INPUT_LAYOUT[] =
				{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				};
				ThrowIfFailed(pDevice->CreateInputLayout(SAMPLING_INPUT_LAYOUT, (sizeof(SAMPLING_INPUT_LAYOUT) / sizeof(D3D11_INPUT_ELEMENT_DESC)), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), lSamplingIL->ReleaseAndGetAddressOf()));
				SAMPLING_IL = lSamplingIL;
			}

			// PIXEL SHADER
			{
				D3D11PixelShader* src[] =
				{
					new D3D11PixelShader(L"BASIC"),
					new D3D11PixelShader(L"CUBEMAP"),
					new D3D11PixelShader(L"FILTER_COMBINE"),
					new D3D11PixelShader(L"FOG"),
					new D3D11PixelShader(L"VOLUME"),
					new D3D11PixelShader(L"DEMO"),
					new D3D11PixelShader(L"OCEAN_SURFACE"),
				};

				RPixelShader** dst[] =
				{
					&BASIC_PS,
					&CUBEMAP_PS,
					&FILTER_COMBINE_PS,
					&FOG_PS,
					&VOLUME_PS,
					&DEMO_PS,
					&OCEAN_SURFACE_PS
				};
				static_assert(sizeof(src) == sizeof(dst));

				for (UINT i = 0; i < sizeof(dst) / sizeof(void*); ++i)
				{
					swprintf(shaderFileNameBuffer, BUFFER_COUNT, L"%s%s_%s.hlsl", BASE_PATH, src[i]->GetName(), ToString(EShaderType::PIXEL_SHADER));
					ThrowIfFailed(CompileShader(shaderFileNameBuffer, "main", "ps_5_0", NULL, shaderBlob.GetAddressOf()));
					ThrowIfFailed(pDevice->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, src[i]->ReleaseAndGetAddressOf()));

					*dst[i] = src[i];
				}
			}


			// COMPUTE SHADER
			{
				D3D11ComputeShader* src[] =
				{
					new D3D11ComputeShader(L"DOWN_BLUR"),
					new D3D11ComputeShader(L"UP_BLUR"),
					new D3D11ComputeShader(L"CLOUD_DENSITY"),
					new D3D11ComputeShader(L"CLOUD_LIGHTING"),
					new D3D11ComputeShader(L"OCEAN_INITIAL_SPECTRUM"),
					new D3D11ComputeShader(L"OCEAN_TIME_DEPENDENT_SPECTRUM"),
					new D3D11ComputeShader(L"OCEAN_FFT"),
					new D3D11ComputeShader(L"OCEAN_FFT_POST_PROCESS"),
					new D3D11ComputeShader(L"OCEAN_COMBINE_WAVE"),
					new D3D11ComputeShader(L"OCEAN_FOAM_SIMULATION"),
				};

				RComputeShader** dst[] =
				{
					&DOWN_BLUR_CS,
					&UP_BLUR_CS,
					&CLOUD_DENSITY_CS,
					&CLOUD_LIGHTING_CS,
					&OCEAN_INITIAL_SPECTRUM_CS,
					&OCEAN_TIME_DEPENDENT_SPECTRUM_CS,
					&OCEAN_FFT_CS,
					&OCEAN_FFT_POST_PROCESS_CS,
					&OCEAN_COMBINE_WAVE_CS,
					&OCEAN_FOAM_SIMULATION_CS,
				};
				static_assert(sizeof(src) == sizeof(dst));

				for (UINT i = 0; i < sizeof(dst) / sizeof(void*); ++i)
				{
					swprintf(shaderFileNameBuffer, BUFFER_COUNT, L"%s%s_%s.hlsl", BASE_PATH, src[i]->GetName(), ToString(EShaderType::COMPUTE_SHADER));
					ThrowIfFailed(CompileShader(shaderFileNameBuffer, "main", "cs_5_0", NULL, shaderBlob.GetAddressOf()));
					ThrowIfFailed(pDevice->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, src[i]->ReleaseAndGetAddressOf()));

					*dst[i] = src[i];
				}
			}

			// HULL SHADER
			{
				D3D11HullShader* lTessellatedQuadHS = new D3D11HullShader(L"TESSELLATED_QUAD");

				swprintf(shaderFileNameBuffer, BUFFER_COUNT, L"%s%s_%s.hlsl", BASE_PATH, lTessellatedQuadHS->GetName(), ToString(EShaderType::HULL_SHADER));
				ThrowIfFailed(CompileShader(shaderFileNameBuffer, "main", "hs_5_0", NULL, shaderBlob.GetAddressOf()));
				ThrowIfFailed(pDevice->CreateHullShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, lTessellatedQuadHS->ReleaseAndGetAddressOf()));

				TESSELLATED_QUAD_HS = lTessellatedQuadHS;
			}

			// DOMAIN SHADER
			{
				D3D11DomainShader* lTessellatedQuadDS = new D3D11DomainShader(L"TESSELLATED_QUAD");

				swprintf(shaderFileNameBuffer, BUFFER_COUNT, L"%s%s_%s.hlsl", BASE_PATH, lTessellatedQuadDS->GetName(), ToString(EShaderType::DOMAIN_SHADER));
				ThrowIfFailed(CompileShader(shaderFileNameBuffer, "main", "ds_5_0", NULL, shaderBlob.GetAddressOf()));
				ThrowIfFailed(pDevice->CreateDomainShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, lTessellatedQuadDS->ReleaseAndGetAddressOf()));

				TESSELATED_QUAD_DS = lTessellatedQuadDS;
			}

		}
		void InitSamplerStates(const D3D11Renderer* pRenderer)
		{
			auto* pDevice = pRenderer->GetDeviceResources()->GetD3DDevice();
			D3D11_SAMPLER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			desc.MinLOD = 0;
			desc.MaxLOD = D3D11_FLOAT32_MAX;

			auto* lLinearWrapSS = new RSamplerState();
			auto* lLinearClampSS = new RSamplerState();
			auto* lLinearMirrorSS = new RSamplerState();
			auto* lShadowPointSS = new RSamplerState();
			auto* lShadowCompareSS = new RSamplerState();

			ThrowIfFailed(pDevice->CreateSamplerState(&desc, lLinearWrapSS->ReleaseAndGetAddressOf()));

			desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			ThrowIfFailed(pDevice->CreateSamplerState(&desc, lLinearClampSS->ReleaseAndGetAddressOf()));

			desc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
			ThrowIfFailed(pDevice->CreateSamplerState(&desc, lLinearClampSS->ReleaseAndGetAddressOf()));

			desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
			desc.BorderColor[0] = 300.0f;
			desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

			ThrowIfFailed(pDevice->CreateSamplerState(&desc, lShadowPointSS->ReleaseAndGetAddressOf()));

			desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
			ThrowIfFailed(pDevice->CreateSamplerState(&desc, lShadowCompareSS->ReleaseAndGetAddressOf()));

			LINEAR_WRAP_SS = lLinearWrapSS;
			LINEAR_CLAMP_SS = lLinearClampSS;
			LINEAR_MIRROR_SS = lLinearMirrorSS;
			SHADOW_COMPARE_SS = lShadowCompareSS;
			SHADOW_POINT_SS = lShadowPointSS;

		}

		void InitBlendStates(const D3D11Renderer* pRenderer)
		{
			auto* pDevice = pRenderer->GetDeviceResources()->GetD3DDevice();

			RBlendState* lAlphaBS = new RBlendState();

			D3D11_BLEND_DESC blendDesc;
			ZeroMemory(&blendDesc, sizeof(blendDesc));
			blendDesc.AlphaToCoverageEnable = false;
			blendDesc.IndependentBlendEnable = false;
			blendDesc.RenderTarget[0].BlendEnable = true;
			blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;


			ThrowIfFailed(pDevice->CreateBlendState(&blendDesc, lAlphaBS->ReleaseAndGetAddressOf()));
			ALPHA_BS = lAlphaBS;
		}

		void InitRasterizerStates(const D3D11Renderer* pRenderer)
		{
			auto* pDevice = pRenderer->GetDeviceResources()->GetD3DDevice();

			auto* lSolidCwRS = new D3D11RasterizerState();
			auto* lSolidCcwRS = new D3D11RasterizerState();
			auto* lWireFrameCwRS = new D3D11RasterizerState();
			auto* lWireFrameCcwRS = new D3D11RasterizerState();

			D3D11_RASTERIZER_DESC rastDesc;
			ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
			rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
			rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
			rastDesc.FrontCounterClockwise = false;
			rastDesc.DepthClipEnable = true;

			ThrowIfFailed(pDevice->CreateRasterizerState(&rastDesc, lSolidCwRS->ReleaseAndGetAddressOf()));

			rastDesc.FrontCounterClockwise = true;
			ThrowIfFailed(pDevice->CreateRasterizerState(&rastDesc, lSolidCcwRS->ReleaseAndGetAddressOf()));

			rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
			ThrowIfFailed(pDevice->CreateRasterizerState(&rastDesc, lWireFrameCcwRS->ReleaseAndGetAddressOf()));

			rastDesc.FrontCounterClockwise = false;
			ThrowIfFailed(pDevice->CreateRasterizerState(&rastDesc, lWireFrameCwRS->ReleaseAndGetAddressOf()));


			SOLID_CW_RS = lSolidCwRS;
			SOLID_CCW_RS = lSolidCcwRS;
			WIRE_FRAME_CW_RS = lWireFrameCwRS;
			WIRE_FRAME_CCW_RS = lWireFrameCcwRS;
		}

		void InitDepthStencilStates(const D3D11Renderer* pRenderer)
		{

		}

		void ClearCommonResources() // Delete All
		{
			delete BASIC_VS;
			delete CUBEMAP_VS;
			delete QUAD_VS;
			delete DEPTH_ONLY_VS;
			delete SAMPLING_VS;


			delete BASIC_PS;
			delete CUBEMAP_PS;
			delete FILTER_COMBINE_PS;
			delete FOG_PS;
			delete VOLUME_PS;
			delete OCEAN_SURFACE_PS;


			delete DOWN_BLUR_CS;
			delete UP_BLUR_CS;
			delete CLOUD_DENSITY_CS;
			delete CLOUD_LIGHTING_CS;
			delete OCEAN_INITIAL_SPECTRUM_CS;
			delete OCEAN_TIME_DEPENDENT_SPECTRUM_CS;
			delete OCEAN_FFT_CS;
			delete OCEAN_FFT_POST_PROCESS_CS;
			delete OCEAN_COMBINE_WAVE_CS;
			delete OCEAN_FOAM_SIMULATION_CS;


			delete TESSELLATED_QUAD_HS;

			delete TESSELATED_QUAD_DS;



			delete LINEAR_WRAP_SS;
			delete LINEAR_CLAMP_SS;
			delete LINEAR_MIRROR_SS;
			delete SHADOW_POINT_SS;
			delete SHADOW_COMPARE_SS;

			delete SOLID_CW_RS;
			delete SOLID_CCW_RS;
			delete WIRE_FRAME_CW_RS;
			delete WIRE_FRAME_CCW_RS;

			delete BASIC_IL;
			delete SAMPLING_IL;

			delete ALPHA_BS;

		}
	}
}