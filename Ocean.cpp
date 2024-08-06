#include "pch.h"

#include "Ocean.h"
#include "GraphicsCommon.h"
#include "Utility.h"

Ocean::Ocean()
	: Model("Ocean", EModelType::OCEAN, Graphics::Ocean::oceanPSO),
	mb_initialized(false),
	m_heightMapCPU{ 0, },
	m_combineWaveConstant(ocean::CombineWaveConstantInitializer),
	m_combineParameters(ocean::CombineParameterInitializer),
	m_oceanConfigurationConstant(ocean::OceanConfigurationInitializer), // what is differnt with {}?
	m_LocalInitialSpectrumParameters(ocean::LocalInitialSpectrumParameterInitializer),
	m_spectrumConstant(ocean::SpectrumConstantInitializer),
	m_FFTConstant(ocean::FFTConstantInitializer),
	m_renderParameter(ocean::RenderingParamsInitialzer)
{
	IGUIComponent::m_type = EGUIType::OCEAN;
}
AObject* Ocean::GetThis()
{
	return this;
}

void Ocean::Initialize(ID3D11Device1* pDevice)
{
	// Textures
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		desc.Width = ocean::N;
		desc.Height = ocean::N;
		desc.MipLevels = 1;
		desc.ArraySize = ocean::CASCADE_COUNT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		// For Timedependent Spectrum
		DX::ThrowIfFailed(pDevice->CreateTexture2D(&desc, NULL, m_displacementMap.GetAddressOf()));
		DX::ThrowIfFailed(pDevice->CreateTexture2D(&desc, NULL, m_derivativeMap.GetAddressOf()));
		DX::ThrowIfFailed(pDevice->CreateTexture2D(&desc, NULL, m_turbulenceMap.GetAddressOf()));

		// Wave vector datas
		DX::ThrowIfFailed(pDevice->CreateTexture2D(&desc, NULL, m_waveVectorData.GetAddressOf()));

		// Initial Spectrum Textures, float2 Texture2DArray
		desc.Format = DXGI_FORMAT_R32G32_FLOAT;
		DX::ThrowIfFailed(pDevice->CreateTexture2D(&desc, NULL, m_initialSpectrumMap.GetAddressOf()));

		desc.Format = DXGI_FORMAT_R32_FLOAT;
		desc.ArraySize = 1;
		DX::ThrowIfFailed(pDevice->CreateTexture2D(&desc, NULL, m_heightMapGPU.GetAddressOf()));

		// Stating HeightMap Texture for CPU
		desc.Usage = D3D11_USAGE_STAGING;
		desc.BindFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		DX::ThrowIfFailed(pDevice->CreateTexture2D(&desc, NULL, m_heightMapGPUStaging.GetAddressOf()));
	}




	// Structured Buffer
	{
		Utility::DXResource::CreateStructuredBuffer(pDevice, sizeof(ocean::InitialSpectrumParameter), ocean::CASCADE_COUNT, &m_LocalInitialSpectrumParameters, m_LocalInitialSpectrumParameterSB.GetAddressOf());
		Utility::DXResource::CreateStructuredBufferSRV(pDevice, m_LocalInitialSpectrumParameterSB.Get(), 0, m_LocalInitialSpectrumParameterSRV.GetAddressOf());

		Utility::DXResource::CreateStructuredBuffer(pDevice, sizeof(ocean::InitialSpectrumParameter), ocean::CASCADE_COUNT, &m_SwellInitialSpectrumParameters, m_SwellInitialSpectrumParameterSB.GetAddressOf());
		Utility::DXResource::CreateStructuredBufferSRV(pDevice, m_SwellInitialSpectrumParameterSB.Get(), 0, m_SwellInitialSpectrumParameterSRV.GetAddressOf());

		Utility::DXResource::CreateStructuredBuffer(pDevice, sizeof(ocean::CombineParameter), ocean::CASCADE_COUNT, &m_combineParameters, m_combineParamterSB.GetAddressOf());
		Utility::DXResource::CreateStructuredBufferSRV(pDevice, m_combineParamterSB.Get(), 0, m_combineParameterSRV.GetAddressOf());
	}


	// UAVs
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		ZeroMemory(&uavDesc, sizeof(uavDesc));
		uavDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
		uavDesc.Texture2DArray.ArraySize = ocean::CASCADE_COUNT;
		uavDesc.Texture2DArray.FirstArraySlice = 0;
		uavDesc.Texture2DArray.MipSlice = 0; // ?? what effect?s

		DX::ThrowIfFailed(pDevice->CreateUnorderedAccessView(m_displacementMap.Get(), &uavDesc, m_displacementMapUAV.GetAddressOf()));
		DX::ThrowIfFailed(pDevice->CreateUnorderedAccessView(m_derivativeMap.Get(), &uavDesc, m_derivativeMapUAV.GetAddressOf()));
		DX::ThrowIfFailed(pDevice->CreateUnorderedAccessView(m_turbulenceMap.Get(), &uavDesc, m_turbulenceMapUAV.GetAddressOf()));
		DX::ThrowIfFailed(pDevice->CreateUnorderedAccessView(m_waveVectorData.Get(), &uavDesc, m_waveVectorDataUAV.GetAddressOf()));

		uavDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		DX::ThrowIfFailed(pDevice->CreateUnorderedAccessView(m_initialSpectrumMap.Get(), &uavDesc, m_initialSpectrumMapUAV.GetAddressOf()));

		// heightMap UAV
		ZeroMemory(&uavDesc, sizeof(uavDesc));
		uavDesc.Format = DXGI_FORMAT_R32_FLOAT;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;
		DX::ThrowIfFailed(pDevice->CreateUnorderedAccessView(m_heightMapGPU.Get(), &uavDesc, m_heightMapUAV.GetAddressOf()));
	}


	// SRVs
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		srvDesc.Texture2DArray.ArraySize = ocean::CASCADE_COUNT;
		srvDesc.Texture2DArray.FirstArraySlice = 0;
		srvDesc.Texture2DArray.MipLevels = 1; // ?
		srvDesc.Texture2DArray.MostDetailedMip = 0; // ?

		DX::ThrowIfFailed(pDevice->CreateShaderResourceView(m_waveVectorData.Get(), &srvDesc, m_waveVectorDataSRV.GetAddressOf()));
		DX::ThrowIfFailed(pDevice->CreateShaderResourceView(m_displacementMap.Get(), &srvDesc, m_displacementMapSRV.GetAddressOf()));
		DX::ThrowIfFailed(pDevice->CreateShaderResourceView(m_derivativeMap.Get(), &srvDesc, m_derivativeMapSRV.GetAddressOf()));
		DX::ThrowIfFailed(pDevice->CreateShaderResourceView(m_turbulenceMap.Get(), &srvDesc, m_turbulenceMapSRV.GetAddressOf()));

		srvDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		DX::ThrowIfFailed(pDevice->CreateShaderResourceView(m_initialSpectrumMap.Get(), &srvDesc, m_initialSpectrumMapSRV.GetAddressOf()));

		ZeroMemory(&srvDesc, sizeof(srvDesc));
		// HeightMap SRVs
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;

		DX::ThrowIfFailed(pDevice->CreateShaderResourceView(m_heightMapGPU.Get(), &srvDesc, m_heightMapSRV.GetAddressOf()));
	}



	Utility::DXResource::CreateConstantBuffer(m_oceanConfigurationConstant, pDevice, m_oceanConfigurationCB);
	Utility::DXResource::CreateConstantBuffer(m_spectrumConstant, pDevice, m_spectrumCB);
	Utility::DXResource::CreateConstantBuffer(m_FFTConstant, pDevice, m_FFTCB);
	Utility::DXResource::CreateConstantBuffer(m_combineWaveConstant, pDevice, m_combineWaveCB);
	Utility::DXResource::CreateConstantBuffer(m_renderParameter, pDevice, m_renderParameterCB);
	m_PSO = Graphics::Ocean::oceanPSO;
	Model::Initialize(pDevice);
}
void Ocean::InitializeData(ID3D11DeviceContext1* context)
{
	// 실행은 되는데 그래픽 디버거에 안잡힘 -> 당연히 첫프레임에만 동작하고 마니까 
	// run initspectrum CS, get initial spectrum map

	Utility::DXResource::UpdateBuffer(context, m_combineParamterSB.Get(), sizeof(ocean::CombineParameter), ocean::CASCADE_COUNT, m_combineParameters.data());
	Utility::DXResource::UpdateBuffer(context, m_LocalInitialSpectrumParameterSB.Get(), sizeof(ocean::InitialSpectrumParameter), ocean::CASCADE_COUNT, m_LocalInitialSpectrumParameters.data());

	Graphics::SetPipelineState(context, Graphics::Ocean::initialSpectrumPSO);
	ID3D11UnorderedAccessView* uavs[3] = { m_initialSpectrumMapUAV.Get(), m_waveVectorDataUAV.Get(), m_turbulenceMapUAV.Get() };
	context->CSSetUnorderedAccessViews(0, 3, uavs, NULL);

	ID3D11ShaderResourceView* srvs[1] = { m_LocalInitialSpectrumParameterSRV.Get() };
	context->CSSetShaderResources(0, 1, srvs);

	Utility::DXResource::UpdateConstantBuffer(m_oceanConfigurationConstant, context, m_oceanConfigurationCB);
	ID3D11Buffer* cbs[1] = { m_oceanConfigurationCB.Get() };
	context->CSSetConstantBuffers(0, 1, cbs);

	context->Dispatch(ocean::GROUP_X, ocean::GROUP_Y, 1);
	Utility::ComputeShaderBarrier(context);

	Model::Update(context);
	mb_initialized = true;
}

void Ocean::OnInitialParameterChanged()
{
	mb_initialized = false;
}
void Ocean::Update(ID3D11DeviceContext1* pContext)
{
	// If wave constant changed, re-run InitData routine
	if (mb_initialized == false)
	{
		InitializeData(pContext);
	}

	Utility::DXResource::UpdateConstantBuffer(m_renderParameter, pContext, m_renderParameterCB);

	// Timedepedent Spectrum, from InitialSpectrum
	{
		// apply delta time, update spectrum map using tilde h0
		// RUN Time dependent sepctrum CS using textures tilde h0(k,t), and waveData that contains wave vector k
		Graphics::SetPipelineState(pContext, Graphics::Ocean::timedependentSpectrumPSO);

		// TEMP
		{
			m_spectrumConstant.time += ocean::TEMP_DELTA_TIME;
			Utility::DXResource::UpdateConstantBuffer(m_spectrumConstant, pContext, m_spectrumCB);
		}

		ID3D11UnorderedAccessView* SpectrumUAVs[2] = { m_displacementMapUAV.Get(), m_derivativeMapUAV.Get() };
		ID3D11ShaderResourceView* SpectrumSRVs[2] = { m_initialSpectrumMapSRV.Get(), m_waveVectorDataSRV.Get() };
		ID3D11Buffer* SpectrumCBs[1] = { m_spectrumCB.Get() };

		pContext->CSSetUnorderedAccessViews(0, sizeof(SpectrumUAVs) / sizeof(ID3D11UnorderedAccessView*), SpectrumUAVs, NULL);
		pContext->CSSetShaderResources(0, sizeof(SpectrumSRVs) / sizeof(ID3D11ShaderResourceView*), SpectrumSRVs);

		Utility::DXResource::UpdateConstantBuffer(m_spectrumConstant, pContext, m_spectrumCB);
		pContext->CSSetConstantBuffers(0, 1, SpectrumCBs);

		pContext->Dispatch(ocean::GROUP_X, ocean::GROUP_X, 1);
		Utility::ComputeShaderBarrier(pContext);
	}


	// Inverse FFT
	{
		// run IFFT CS, renew height map from spectrum map, get normal map from derivative map
		// use FFT texture updated prev time dependent spectrum CS
		// Run FFT CS twice on FFT texture as it is 2D FFT, one with horizontally and the other one with vertically(order can be changed)

		ID3D11Buffer* FFTCBs[1] = { m_FFTCB.Get() };
		ID3D11UnorderedAccessView* FFTUAVs[1] = { m_displacementMapUAV.Get() };

		// horizontal IFFT
		{
			m_FFTConstant.bInverse = TRUE;
			m_FFTConstant.bDirection = FALSE;
			Utility::DXResource::UpdateConstantBuffer(m_FFTConstant, pContext, m_FFTCB);

			pContext->CSSetShaderResources(0, 0, NULL);
			Graphics::SetPipelineState(pContext, Graphics::Ocean::FFTPSO);
			pContext->CSSetConstantBuffers(0, 1, FFTCBs);

			FFTUAVs[0] = m_displacementMapUAV.Get();
			pContext->CSSetUnorderedAccessViews(0, 1, FFTUAVs, NULL);
			pContext->Dispatch(1, ocean::N, 1);
			Utility::ComputeShaderBarrier(pContext);

			FFTUAVs[0] = m_derivativeMapUAV.Get();
			pContext->CSSetUnorderedAccessViews(0, 1, FFTUAVs, NULL);
			pContext->Dispatch(1, ocean::N, 1);
			Utility::ComputeShaderBarrier(pContext);
		}


		// vertical IFFT
		{
			m_FFTConstant.bDirection = TRUE;
			Utility::DXResource::UpdateConstantBuffer(m_FFTConstant, pContext, m_FFTCB);

			Graphics::SetPipelineState(pContext, Graphics::Ocean::FFTPSO);
			pContext->CSSetConstantBuffers(0, 1, FFTCBs);

			FFTUAVs[0] = m_displacementMapUAV.Get();
			pContext->CSSetUnorderedAccessViews(0, 1, FFTUAVs, NULL);
			pContext->Dispatch(1, ocean::N, 1);
			Utility::ComputeShaderBarrier(pContext);

			FFTUAVs[0] = m_derivativeMapUAV.Get();
			pContext->CSSetUnorderedAccessViews(0, 1, FFTUAVs, NULL);
			pContext->Dispatch(1, ocean::N, 1);
			Utility::ComputeShaderBarrier(pContext);
		}

		// IFFT PostProcess, permute odd wave vector
		{
			m_FFTConstant.bPermute = TRUE;
			Utility::DXResource::UpdateConstantBuffer(m_FFTConstant, pContext, m_FFTCB);
			Graphics::SetPipelineState(pContext, Graphics::Ocean::FFTPostProcessPSO);
			pContext->CSSetConstantBuffers(0, 1, FFTCBs);

			// displacement map
			FFTUAVs[0] = m_displacementMapUAV.Get();
			pContext->CSSetUnorderedAccessViews(0, 1, FFTUAVs, NULL);

			pContext->Dispatch(ocean::GROUP_X, ocean::GROUP_Y, 1);
			Utility::ComputeShaderBarrier(pContext);

			// derivative map
			FFTUAVs[0] = m_derivativeMapUAV.Get();
			pContext->CSSetUnorderedAccessViews(0, 1, FFTUAVs, NULL);

			pContext->Dispatch(ocean::GROUP_X, ocean::GROUP_Y, 1);
			Utility::ComputeShaderBarrier(pContext);
		}
	}

	// Foam Simulation
	{
		Graphics::SetPipelineState(pContext, Graphics::Ocean::foamSimulationPSO);

		ID3D11ShaderResourceView* foamSRVs[3] = { m_displacementMapSRV.Get(), m_derivativeMapSRV.Get(), m_combineParameterSRV.Get() };
		pContext->CSSetShaderResources(0, 3, foamSRVs);

		ID3D11UnorderedAccessView* foamUAVs[1] = { m_turbulenceMapUAV.Get() };
		pContext->CSSetUnorderedAccessViews(0, 1, foamUAVs, NULL);

		pContext->Dispatch(ocean::GROUP_X, ocean::GROUP_Y, 1);
		Utility::ComputeShaderBarrier(pContext);
	}

	// Combine wave
	{
		Graphics::SetPipelineState(pContext, Graphics::Ocean::combineWavePSO);
		ID3D11Buffer* combineWaveCBs[1] = { m_combineWaveCB.Get() };

		pContext->CSSetConstantBuffers(0, 1, combineWaveCBs);

		ID3D11UnorderedAccessView* combineWaveUAVs[1] = { m_heightMapUAV.Get() };
		pContext->CSSetUnorderedAccessViews(0, 1, combineWaveUAVs, NULL);

		ID3D11ShaderResourceView* combineWaveSRVs[2] = { m_displacementMapSRV.Get(), m_combineParameterSRV.Get() };
		pContext->CSSetShaderResources(0, 2, combineWaveSRVs);

		ID3D11SamplerState* combineWaveSSs[1] = { Graphics::linearWrapSS.Get() };
		pContext->CSSetSamplers(0, 1, combineWaveSSs);


		pContext->Dispatch(ocean::GROUP_X, ocean::GROUP_Y, 1);
		Utility::ComputeShaderBarrier(pContext);
	}



	// Copy Heightmap GPU -> CPU
	{
		pContext->CopyResource(m_heightMapGPUStaging.Get(), m_heightMapGPU.Get());

		D3D11_MAPPED_SUBRESOURCE resourceDesc = {};

		pContext->Map(m_heightMapGPUStaging.Get(), 0, D3D11_MAP_READ, 0, &resourceDesc); // send data to CPU
		if (resourceDesc.pData)
		{
			const int BytesPerPixel = sizeof(float); //R32 float
			assert(ocean::N * BytesPerPixel == resourceDesc.RowPitch);
			for (unsigned int i = 0; i < ocean::N; ++i)
			{
				// copying row by row
				std::memcpy((byte*)m_heightMapCPU + (ocean::N * BytesPerPixel * i), (byte*)resourceDesc.pData + (resourceDesc.RowPitch * i), static_cast<size_t>(ocean::N * BytesPerPixel));

			}

		}
		pContext->Unmap(m_heightMapGPUStaging.Get(), 0);
	}

	// Model::Update(pContext); 안해도됨
}

void Ocean::Render(ID3D11DeviceContext1* pContext)
{

	ID3D11ShaderResourceView* SRVs[3] = { GetDisplacementMapsSRV(), GetDerivativeMapsSRV(), GetCombineParameterSRV() };
	pContext->DSSetShaderResources(100, 3, SRVs);
	ID3D11ShaderResourceView* foamSRVs[2] = { GetTurbulenceMapsSRV(), GetCombineParameterSRV() };
	pContext->PSSetShaderResources(100, 2, foamSRVs);

	ID3D11SamplerState* SSs[1] = { Graphics::linearWrapSS.Get() };
	pContext->DSSetSamplers(0, 1, SSs);

	pContext->PSSetConstantBuffers(5, 1, m_renderParameterCB.GetAddressOf());

	Model::Render(pContext);

	ID3D11ShaderResourceView* release[6] = { 0, };
	pContext->PSSetShaderResources(100, 2, release);
	pContext->DSSetShaderResources(100, 6, release);
}

void Ocean::RenderOverride(ID3D11DeviceContext1* pContext, const GraphicsPSO& pso)
{
	return;
	//Model::RenderOverride(pContext, pso);
}

float Ocean::GetHeight(DirectX::SimpleMath::Vector2 XZ) const
{
	// OceanPlanetop left -75 75, bottom right 75 - 75, 150 x 150, decided by WORLD_SCALER
	// texture is 512 x 512
	// sim size is 2048m
	// -75, -75가 0, 0이 되어야 한다 75, 75가 1,1. xz 와 uv 방향동일

	DirectX::SimpleMath::Vector2 scaled = XZ + DirectX::SimpleMath::Vector2(ocean::WORLD_SCALER, ocean::WORLD_SCALER); // 150 x 150
	scaled /= 2 * ocean::WORLD_SCALER; // now in uv

	unsigned int x = std::min(static_cast<unsigned int>(ocean::N * scaled.x), ocean::N - 1);
	unsigned int z = std::min(static_cast<unsigned int>(ocean::N * scaled.y), ocean::N - 1);

	constexpr int offsets[5][2] =
	{
		{0, -1},
		{-1, 0},
		{0, 1},
		{1, 0},
		{0, 0}
	};

	float height = 0.f;
	for (const int* offset : offsets)
	{
		unsigned int multiZ = std::min(std::max(0u, z + offset[0]), ocean::N - 1);
		unsigned int multiX = std::min(std::max(0u, x + offset[1]), ocean::N - 1);

		height += m_heightMapCPU[multiZ][multiX];
	}

	return height / 5.f;
}


void Ocean::UpdateCombineParameter(const std::array<ocean::CombineParameter, ocean::CASCADE_COUNT>& updatedCombineParameters)
{
	for (unsigned int i = 0; i < ocean::CASCADE_COUNT; ++i)
	{
		m_combineParameters[i] = updatedCombineParameters[i];
	}
}
void Ocean::UpdateInitialSpectrumParameter(const std::array<ocean::InitialSpectrumParameter, ocean::CASCADE_COUNT>& updatedInitialSpectrumParameters)
{
	for (unsigned int i = 0; i < ocean::CASCADE_COUNT; ++i)
	{
		m_LocalInitialSpectrumParameters[i] = updatedInitialSpectrumParameters[i];
	}
}
void Ocean::UpdateOceanConfiguration(const ocean::OceanConfigurationConstant& updatedOceanConfig)
{
	m_oceanConfigurationConstant = updatedOceanConfig;
}
void Ocean::UpdateRenderingParameter(const ocean::RenderingParameter& renderingParam)
{
	m_renderParameter = renderingParam;
}