#include "pch.h"
#include "Ocean.h"
#include "GraphicsCommon.h"
#include "Utility.h"

Ocean::Ocean(ID3D11Device1* device)
	:mb_initialized(false),
	m_heightMapCPU{ 0, },
	m_initialSpectrumWaveConstant(ocean::InitialSpectrumWaveConstantInitializer), // what is differnt with {}?
	m_LocalInitialSpectrumParameterConstant(ocean::LocalInitialSpectrumParameterConstantInitializer),
	m_spectrumConstant(ocean::SpectrumConstantInitializer),
	m_FFTConstant(ocean::FFTConstantInitializer)
{
	// create d3d resources

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
		DX::ThrowIfFailed(device->CreateTexture2D(&desc, NULL, m_displacementMap.GetAddressOf()));
		DX::ThrowIfFailed(device->CreateTexture2D(&desc, NULL, m_derivativeMap.GetAddressOf()));

		// Wave vector datas
		DX::ThrowIfFailed(device->CreateTexture2D(&desc, NULL, m_waveVectorData.GetAddressOf()));

		// Initial Spectrum Textures, float2 Texture2DArray
		desc.Format = DXGI_FORMAT_R32G32_FLOAT;
		DX::ThrowIfFailed(device->CreateTexture2D(&desc, NULL, m_initialSpectrumMap.GetAddressOf()));

		// Combined Result, Normal and HeightMap
		desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		desc.ArraySize = 1;
		desc.BindFlags = 0;
		DX::ThrowIfFailed(device->CreateTexture2D(&desc, NULL, m_normalMap.GetAddressOf()));

		desc.Format = DXGI_FORMAT_R32_FLOAT;
		DX::ThrowIfFailed(device->CreateTexture2D(&desc, NULL, m_heightMapGPU.GetAddressOf()));

		// Stating HeightMap Texture for CPU
		desc.Usage = D3D11_USAGE_STAGING;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		DX::ThrowIfFailed(device->CreateTexture2D(&desc, NULL, m_heightMapGPUStaging.GetAddressOf()));
	}




	// Structured Buffer
	{
		Utility::DXResource::CreateStructuredBuffer(device, sizeof(ocean::InitialSpectrumParameterConstant), ocean::CASCADE_COUNT, &m_LocalInitialSpectrumParameterConstant, m_LocalInitialSpectrumParameterSB.GetAddressOf());
		Utility::DXResource::CreateBufferSRV(device, m_LocalInitialSpectrumParameterSB.Get(), m_LocalInitialSpectrumParameterSRV.GetAddressOf());

		Utility::DXResource::CreateStructuredBuffer(device, sizeof(ocean::InitialSpectrumParameterConstant), ocean::CASCADE_COUNT, &m_SwellInitialSpectrumParameterConstant, m_SwellInitialSpectrumParameterSB.GetAddressOf());
		Utility::DXResource::CreateBufferSRV(device, m_SwellInitialSpectrumParameterSB.Get(), m_SwellInitialSpectrumParameterSRV.GetAddressOf());

		// TODO : CombineWave에서 쓰는거
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

		DX::ThrowIfFailed(device->CreateUnorderedAccessView(m_displacementMap.Get(), &uavDesc, m_displacementMapUAV.GetAddressOf()));
		DX::ThrowIfFailed(device->CreateUnorderedAccessView(m_derivativeMap.Get(), &uavDesc, m_derivativeMapUAV.GetAddressOf()));
		DX::ThrowIfFailed(device->CreateUnorderedAccessView(m_waveVectorData.Get(), &uavDesc, m_waveVectorDataUAV.GetAddressOf()));

		uavDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		DX::ThrowIfFailed(device->CreateUnorderedAccessView(m_initialSpectrumMap.Get(), &uavDesc, m_initialSpectrumMapUAV.GetAddressOf()));

		// TODO : heightMap, Normal Map UAV
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

		DX::ThrowIfFailed(device->CreateShaderResourceView(m_waveVectorData.Get(), &srvDesc, m_waveVectorDataSRV.GetAddressOf()));

		srvDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		DX::ThrowIfFailed(device->CreateShaderResourceView(m_initialSpectrumMap.Get(), &srvDesc, m_initialSpectrumMapSRV.GetAddressOf()));

		// TODO : HeightMap, NormalMap SRVs
	}



	Utility::DXResource::CreateConstantBuffer(m_initialSpectrumWaveConstant, device, m_initialSpectrumWaveCB);
	Utility::DXResource::CreateConstantBuffer(m_spectrumConstant, device, m_spectrumCB);
	Utility::DXResource::CreateConstantBuffer(m_FFTConstant, device, m_FFTCB);


}

void Ocean::Initialize(ID3D11DeviceContext1* context)
{
	// 실행은 되는데 그래픽 디버거에 안잡힘 -> 당연히 첫프레임에만 동작하고 마니까 
	// run initspectrum CS, get initial spectrum map
	Graphics::SetPipelineState(context, Graphics::Ocean::initialSpectrumPSO);
	ID3D11UnorderedAccessView* uavs[2] = { m_initialSpectrumMapUAV.Get(), m_waveVectorDataUAV.Get() };
	context->CSSetUnorderedAccessViews(0, 2, uavs, NULL);

	ID3D11ShaderResourceView* srvs[1] = { m_LocalInitialSpectrumParameterSRV.Get() };
	context->CSSetShaderResources(0, 1, srvs);

	Utility::DXResource::UpdateConstantBuffer(m_initialSpectrumWaveConstant, context, m_initialSpectrumWaveCB);
	ID3D11Buffer* cbs[1] = { m_initialSpectrumWaveCB.Get() };
	context->CSSetConstantBuffers(0, 1, cbs);

	context->Dispatch(ocean::GROUP_X, ocean::GROUP_Y, 1);
	Utility::ComputeShaderBarrier(context);

	// TODO : foam simulations

#ifdef NDEBUG
	mb_initialized = true;
#else
	mb_initialized = false;
#endif
}

void Ocean::Update(ID3D11DeviceContext1* context)
{
	// If wave constant changed, re-run InitData routine
	if (mb_initialized == false)
	{
		Initialize(context);
	}

	// Timedepedent Spectrum, from InitialSpectrum
	{
		// apply delta time, update spectrum map using tilde h0
		// RUN Time dependent sepctrum CS using textures tilde h0(k,t), and waveData that contains wave vector k
		Graphics::SetPipelineState(context, Graphics::Ocean::timedependentSpectrumPSO);
		ID3D11UnorderedAccessView* SpectrumUAVs[2] = { m_displacementMapUAV.Get(), m_derivativeMapUAV.Get() };
		ID3D11ShaderResourceView* SpectrumSRVs[2] = { m_initialSpectrumMapSRV.Get(), m_waveVectorDataSRV.Get() };
		ID3D11Buffer* SpectrumCBs[1] = { m_spectrumCB.Get() };

		context->CSSetUnorderedAccessViews(0, sizeof(SpectrumUAVs) / sizeof(ID3D11UnorderedAccessView*), SpectrumUAVs, NULL);
		context->CSSetShaderResources(0, sizeof(SpectrumSRVs) / sizeof(ID3D11ShaderResourceView*), SpectrumSRVs);

		Utility::DXResource::UpdateConstantBuffer(m_spectrumConstant, context, m_spectrumCB);
		context->CSSetConstantBuffers(0, 1, SpectrumCBs);

		context->Dispatch(ocean::GROUP_X, ocean::GROUP_X, 1);
		Utility::ComputeShaderBarrier(context);
	}


	// Inverse FFT
	{
		// run IFFT CS, renew height map from spectrum map, get normal map from height map
		// use FFT texture updated prev time dependent spectrum CS
		// Run FFT CS twice on FFT texture as it is 2D FFT, one with horizontally and the other one with vertically(order can be changed)

		ID3D11Buffer* FFTCBs[1] = { m_FFTCB.Get() };
		ID3D11UnorderedAccessView* FFTUAVs[1] = { m_displacementMapUAV.Get() };

		// horizontal IFFT
		{
			m_FFTConstant.bInverse = true;
			m_FFTConstant.bDirection = false;
			Utility::DXResource::UpdateConstantBuffer(m_FFTConstant, context, m_FFTCB);

			context->CSSetShaderResources(0, 0, NULL);
			Graphics::SetPipelineState(context, Graphics::Ocean::FFTPSO);
			context->CSSetConstantBuffers(0, 1, FFTCBs);

			FFTUAVs[0] = m_displacementMapUAV.Get();
			context->CSSetUnorderedAccessViews(0, 1, FFTUAVs, NULL);
			context->Dispatch(1, ocean::N, 1);
			Utility::ComputeShaderBarrier(context);

			FFTUAVs[0] = m_derivativeMapUAV.Get();
			context->CSSetUnorderedAccessViews(0, 1, FFTUAVs, NULL);
			context->Dispatch(1, ocean::N, 1);
			Utility::ComputeShaderBarrier(context);
		}


		// vertical IFFT
		{
			m_FFTConstant.bDirection = true;
			Utility::DXResource::UpdateConstantBuffer(m_FFTConstant, context, m_FFTCB);

			Graphics::SetPipelineState(context, Graphics::Ocean::FFTPSO);
			context->CSSetConstantBuffers(0, 1, FFTCBs);

			FFTUAVs[0] = m_displacementMapUAV.Get();
			context->CSSetUnorderedAccessViews(0, 1, FFTUAVs, NULL);
			context->Dispatch(1, ocean::N, 1);
			Utility::ComputeShaderBarrier(context);

			FFTUAVs[0] = m_derivativeMapUAV.Get();
			context->CSSetUnorderedAccessViews(0, 1, FFTUAVs, NULL);
			context->Dispatch(1, ocean::N, 1);
			Utility::ComputeShaderBarrier(context);
		}
	}

	// TODO : Run Combine Wave, or Postprocess CS, FFT결과에 PostProcess 필요한가?


	// Copy Heightmap GPU -> CPU
	{
		context->CopyResource(m_heightMapGPUStaging.Get(), m_heightMapGPU.Get());

		D3D11_MAPPED_SUBRESOURCE resourceDesc = {};
		context->Map(m_heightMapGPUStaging.Get(), 0, D3D11_MAP_READ, 0, &resourceDesc); // send data to CPU
		if (resourceDesc.pData)
		{
			const int BytesPerPixel = sizeof(float); //R32 float
			// 그냥 한방에 복사가 안되나??
			for (unsigned int i = 0; i < ocean::N; ++i)
			{
				// copying row by row
				std::memcpy((byte*)m_heightMapCPU + (ocean::N * BytesPerPixel * i), (byte*)resourceDesc.pData + (resourceDesc.RowPitch * i), static_cast<size_t>(ocean::N * BytesPerPixel));
				// assert (ocean::N * BytesPerPixel == resourceDesc.RowPitch);
			}

		}
		context->Unmap(m_heightMapGPUStaging.Get(), 0);
	}



	// TODO : run Foam Simulation on height map(Result IFFT Texture), get Turbulence Map using Jacobian and displacement
}

void Ocean::Draw(ID3D11DeviceContext1* context)
{
	// Set vertex shader <- resources : Height map, displacement map
	// m_oceanPlane.Draw(context);
	// Set Pixel Shader <- resources : Normal map, Jacobian Map, PBR constants
}
