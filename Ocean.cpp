#include "pch.h"
#include "Ocean.h"
#include "GraphicsCommon.h"
#include "Utility.h"

Ocean::Ocean(ID3D11Device1* device)
	:mb_initialized(false),
	m_heightMapCPU{ 0, },
	m_initialSpectrumWaveConstant(ocean::InitialSpectrumWaveConstantInitializer), // what is differnt with {}?
	m_initialSpectrumParameterConstant(ocean::InitialSpectrumParameterConstantInitializer),
	m_spectrumConstant(ocean::SpectrumConstantInitializer),
	m_FFTConstant(ocean::FFTConstantInitializer)
{
	// create d3d resources
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.Width = ocean::N;
	desc.Height = ocean::N;
	desc.MipLevels = 1;
	desc.ArraySize = ocean::CASCADE_COUNT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.MiscFlags = 0;

	// Stating HeightMap Texture for CPU
	DX::ThrowIfFailed(device->CreateTexture2D(&desc, NULL, m_heightMapGPU.GetAddressOf()));

	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	// Spectrum Map Texture
	DX::ThrowIfFailed(device->CreateTexture2D(&desc, NULL, m_spectrumMap.GetAddressOf()));
	DX::ThrowIfFailed(device->CreateTexture2D(&desc, NULL, m_spectrumDerivativeMap.GetAddressOf()));

	// Wave vector datas
	DX::ThrowIfFailed(device->CreateTexture2D(&desc, NULL, m_waveVectorData.GetAddressOf()));

	// Initial Spectrum Textures, float2 Texture2DArray
	desc.Format = DXGI_FORMAT_R16G16_FLOAT;
	DX::ThrowIfFailed(device->CreateTexture2D(&desc, NULL, m_initialSpectrumMap.GetAddressOf()));

	// Structured Buffer
	Utility::DXResource::CreateStructuredBuffer(device, sizeof(ocean::InitialSpectrumParameterConstant), 2 * ocean::CASCADE_COUNT, &m_initialSpectrumParameterConstant, m_initialSpectrumParameterSB.GetAddressOf());
	Utility::DXResource::CreateBufferSRV(device, m_initialSpectrumParameterSB.Get(), m_initialSpectrumParameterSRV.GetAddressOf());

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
	uavDesc.Texture2DArray.ArraySize = ocean::CASCADE_COUNT;
	uavDesc.Texture2DArray.FirstArraySlice = 0;
	uavDesc.Texture2DArray.MipSlice = 0; // ?? what effect?s

	DX::ThrowIfFailed(device->CreateUnorderedAccessView(m_spectrumMap.Get(), &uavDesc, m_spectrumMapUAV.GetAddressOf()));
	DX::ThrowIfFailed(device->CreateUnorderedAccessView(m_spectrumDerivativeMap.Get(), &uavDesc, m_spectrumDerivativeMapUAV.GetAddressOf()));
	DX::ThrowIfFailed(device->CreateUnorderedAccessView(m_waveVectorData.Get(), &uavDesc, m_waveVectorDataUAV.GetAddressOf()));

	uavDesc.Format = DXGI_FORMAT_R16G16_FLOAT;
	DX::ThrowIfFailed(device->CreateUnorderedAccessView(m_initialSpectrumMap.Get(), &uavDesc, m_initialSpectrumMapUAV.GetAddressOf()));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.ArraySize = ocean::CASCADE_COUNT;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.MipLevels = 1; // ?
	srvDesc.Texture2DArray.MostDetailedMip = 0; // ?

	DX::ThrowIfFailed(device->CreateShaderResourceView(m_waveVectorData.Get(), &srvDesc, m_waveVectorDataSRV.GetAddressOf()));

	srvDesc.Format = DXGI_FORMAT_R16G16_FLOAT;
	DX::ThrowIfFailed(device->CreateShaderResourceView(m_initialSpectrumMap.Get(), &srvDesc, m_initialSpectrumMapSRV.GetAddressOf()));



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

	ID3D11ShaderResourceView* srvs[1] = { m_initialSpectrumParameterSRV.Get() };
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

	// apply delta time, update spectrum map using tilde h0
	// RUN Time dependent sepctrum CS using textures tilde h0(k,t), and waveData that contains wave vector k
	Graphics::SetPipelineState(context, Graphics::Ocean::timedependentSpectrumPSO);
	ID3D11UnorderedAccessView* spectrumMapUAV[2] = { m_spectrumMapUAV.Get(), m_spectrumDerivativeMapUAV.Get() };
	context->CSSetUnorderedAccessViews(0, sizeof(spectrumMapUAV) / sizeof(ID3D11UnorderedAccessView*), spectrumMapUAV, NULL);

	ID3D11ShaderResourceView* srvs[2] = { m_initialSpectrumMapSRV.Get(), m_waveVectorDataSRV.Get() };
	context->CSSetShaderResources(0, sizeof(srvs) / sizeof(ID3D11ShaderResourceView*), srvs);

	Utility::DXResource::UpdateConstantBuffer(m_spectrumConstant, context, m_spectrumCB);
	ID3D11Buffer* cbs[1] = { m_spectrumCB.Get() };
	context->CSSetConstantBuffers(0, 1, cbs);

	context->Dispatch(ocean::GROUP_X, ocean::GROUP_X, 1);
	Utility::ComputeShaderBarrier(context);
	// save result into FFT texture for IFFT CS to use it 






	// run IFFT CS, renew height map from spectrum map, get normal map from height map
	// use FFT texture updated prev time dependent spectrum CS
	// Run FFT CS twice on FFT texture as it is 2D FFT, one with horizontally and the other one with vertically(order can be changed)
	cbs[0] = m_FFTCB.Get();

	m_FFTConstant.bInverse = true;
	m_FFTConstant.bDirection = false;
	Utility::DXResource::UpdateConstantBuffer(m_FFTConstant, context, m_FFTCB);

	// horizontally
	{
		context->CSSetShaderResources(0, 0, NULL);
		Graphics::SetPipelineState(context, Graphics::Ocean::FFTPSO);
		context->CSSetUnorderedAccessViews(0, 1, spectrumMapUAV, NULL);
		context->CSSetConstantBuffers(0, 1, cbs);
		context->Dispatch(1, ocean::N, 1); // TODO : 이거 맞는지 확인
		Utility::ComputeShaderBarrier(context);
	}


	m_FFTConstant.bDirection = !m_FFTConstant.bDirection;
	Utility::DXResource::UpdateConstantBuffer(m_FFTConstant, context, m_FFTCB);

	// vertically
	{
		Graphics::SetPipelineState(context, Graphics::Ocean::FFTPSO);
		context->CSSetUnorderedAccessViews(0, 1, spectrumMapUAV, NULL);
		context->CSSetConstantBuffers(0, 1, cbs);
		context->Dispatch(ocean::N, 1, 1);
		Utility::ComputeShaderBarrier(context);
	}

	Utility::ComputeShaderBarrier(context);


	context->CopyResource(m_heightMapGPU.Get(), m_spectrumMap.Get()); // Copy Resource spectrumMap to heightMap Staging Texture

	D3D11_MAPPED_SUBRESOURCE resourceDesc = {};
	context->Map(m_heightMapGPU.Get(), 0, D3D11_MAP_READ, 0, &resourceDesc); // send data to CPU
	if (resourceDesc.pData)
	{
		const int BytesPerPixel = sizeof(uint64_t); //R16G16B16A16 float4

		for (unsigned int depth = 0; depth < ocean::CASCADE_COUNT; depth++)
		{
			void* pTex2DArrayElement = (byte*)resourceDesc.pData + (BytesPerPixel * ocean::N * ocean::N) * depth;
			void* dest = (void*)m_heightMapCPU[depth];
			for (unsigned int i = 0; i < ocean::N; ++i)
			{
				// copying row by row
				std::memcpy((byte*)dest + (ocean::N * BytesPerPixel * i), (byte*)pTex2DArrayElement + (resourceDesc.RowPitch * i), static_cast<size_t>(ocean::N * BytesPerPixel));
			}
		}

		for (unsigned int i = 0; i < ocean::N; ++i)
		{
			for (unsigned int j = 0; j < ocean::N; ++j)
			{
				// TODO : parallel for?
				m_heightMapCPU[0][i][j] = (m_heightMapCPU[0][i][j] + m_heightMapCPU[1][i][j] + m_heightMapCPU[2][i][j] + m_heightMapCPU[3][i][j]) / ocean::CASCADE_COUNT;
			}
		}
	}
	context->Unmap(m_heightMapGPU.Get(), 0);


	// TODO : run Foam Simulation on height map(Result IFFT Texture), get Turbulence Map using Jacobian and displacement
}

void Ocean::Draw(ID3D11DeviceContext1* context)
{
	// Set vertex shader <- resources : Height map, displacement map
	// m_oceanPlane.Draw(context);
	// Set Pixel Shader <- resources : Normal map, Jacobian Map, PBR constants
}
