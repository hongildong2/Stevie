#include "pch.h"
#include "Ocean.h"
#include "GraphicsCommon.h"
#include "Utility.h"

Ocean::Ocean(ID3D11Device1* device)
	:mb_initialized(false),
	m_heightMapCPU{ 0, },
	m_initialSpectrumConstant{ {0.f, 0.f, 0.f , 0.f}, 1.0f, 0.0f, 9.8f, 3.0f },
	m_spectrumConstant{ 0.f, {} },
	m_FFTConstant{ CASCADE_COUNT, 0, 0, 1, 0 }
{
	// create d3d resources
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.Width = N;
	desc.Height = N;
	desc.MipLevels = 0;
	desc.ArraySize = CASCADE_COUNT;
	desc.SampleDesc.Count = 0;
	desc.SampleDesc.Quality = 0;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.MiscFlags = 0;

	// Stating HeightMap Texture for CPU
	DX::ThrowIfFailed(device->CreateTexture2D(&desc, NULL, m_heightMapGPU.GetAddressOf()));

	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	// Spectrum Map Texture
	DX::ThrowIfFailed(device->CreateTexture2D(&desc, NULL, m_spectrumMap.GetAddressOf()));

	// Wave vector datas
	DX::ThrowIfFailed(device->CreateTexture2D(&desc, NULL, m_waveVectorData.GetAddressOf()));

	// Initial Spectrum Textures, float2 Texture2DArray
	desc.Format = DXGI_FORMAT_R16G16_FLOAT;
	DX::ThrowIfFailed(device->CreateTexture2D(&desc, NULL, m_initialSpectrumMap.GetAddressOf()));

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
	uavDesc.Texture2DArray.ArraySize = CASCADE_COUNT;
	uavDesc.Texture2DArray.FirstArraySlice = 0;
	uavDesc.Texture2DArray.MipSlice = 0; // ?? what effect?s


	DX::ThrowIfFailed(device->CreateUnorderedAccessView(m_spectrumMap.Get(), &uavDesc, m_spectrumMapUAV.GetAddressOf()));
	DX::ThrowIfFailed(device->CreateUnorderedAccessView(m_waveVectorData.Get(), &uavDesc, m_waveVectorDataUAV.GetAddressOf()));

	uavDesc.Format = DXGI_FORMAT_R16G16_FLOAT;
	DX::ThrowIfFailed(device->CreateUnorderedAccessView(m_initialSpectrumMap.Get(), &uavDesc, m_initialSpectrumMapUAV.GetAddressOf()));


	Utility::DXResource::CreateConstantBuffer(m_initialSpectrumConstant, device, m_initialSpectrumCB);
	Utility::DXResource::CreateConstantBuffer(m_spectrumConstant, device, m_spectrumCB);
	Utility::DXResource::CreateConstantBuffer(m_FFTConstant, device, m_FFTCB);
}

void Ocean::Initialize(ID3D11DeviceContext1* context)
{
	// run initspectrum CS, get initial spectrum map
	Graphics::SetPipelineState(context, Graphics::Ocean::initialSpectrumPSO);
	ID3D11UnorderedAccessView* uavs[] = { m_initialSpectrumMapUAV.Get(), m_waveVectorDataUAV.Get() };
	context->CSSetUnorderedAccessViews(0, 2, uavs, NULL); // TODO : create shader resource view for textures

	context->Dispatch(GROUP_X, GROUP_Y, 1);

	// get initial wave vectors

	// TODO : foam simulations

}

void Ocean::Update(ID3D11DeviceContext1* context)
{
	// If wave constant changed, re-run InitData routine
	if (mb_initialized == false)
	{
		Initialize(context);
		mb_initialized = true;
	}

	// apply delta time, update spectrum map using tilde h0
	// RUN Time dependent sepctrum CS using textures tilde h0(k,t), and waveData that contains wave vector k
	Graphics::SetPipelineState(context, Graphics::Ocean::timedependentSpectrumPSO);
	ID3D11UnorderedAccessView* spectrumMapUAV[1] = { m_spectrumMapUAV.Get() };
	ID3D11ShaderResourceView* srvs[2] = { m_initialSpectrumMapSRV.Get(), m_waveVectorDataSRV.Get() };
	context->CSSetUnorderedAccessViews(0, sizeof(spectrumMapUAV) / sizeof(ID3D11UnorderedAccessView*), spectrumMapUAV, NULL);
	context->CSSetShaderResources(0, sizeof(srvs) / sizeof(ID3D11ShaderResourceView*), srvs);
	context->Dispatch(GROUP_X, GROUP_X, 1);
	// save result into FFT texture for IFFT CS to use it 


	// run IFFT CS, renew height map from spectrum map, get normal map from height map
	// use FFT texture updated prev time dependent spectrum CS
	// Run FFT CS twice on FFT texture as it is 2D FFT, one with horizontally and the other one with vertically(order can be changed)

	// horizontally
	context->CSSetShaderResources(0, 0, NULL);
	Graphics::SetPipelineState(context, Graphics::Ocean::FFTPSO);
	context->CSSetUnorderedAccessViews(0, 1, spectrumMapUAV, NULL);
	context->Dispatch(1, 1, 1);

	// vertically
	Graphics::SetPipelineState(context, Graphics::Ocean::FFTPSO);
	context->CSSetUnorderedAccessViews(0, 1, spectrumMapUAV, NULL);
	context->Dispatch(1, 1, 1);

	// Copy Resource spectrumMap to heightMap Staging Texture

	context->CSSetUnorderedAccessViews(0, 0, NULL, NULL);
	context->CopyResource(m_heightMapGPU.Get(), m_spectrumMap.Get());
	// send data to CPU

	D3D11_MAPPED_SUBRESOURCE resourceDesc = {};
	context->Map(m_heightMapGPU.Get(), 0, D3D11_MAP_READ, 0, &resourceDesc);


	if (resourceDesc.pData)
	{
		const int BytesPerPixel = sizeof(uint64_t); //R16G16B16A16 float4

		for (unsigned int depth = 0; depth < CASCADE_COUNT; depth++)
		{
			void* pTex2DArrayElement = (byte*)resourceDesc.pData + (BytesPerPixel * N * N) * depth;
			void* dest = (void*)m_heightMapCPU[depth];
			for (unsigned int i = 0; i < N; ++i)
			{
				// copying row by row
				std::memcpy((byte*)dest + (N * BytesPerPixel * i), (byte*)pTex2DArrayElement + (resourceDesc.RowPitch * i), static_cast<size_t>(N * BytesPerPixel));
			}
		}

		for (unsigned int i = 0; i < N; ++i)
		{
			for (unsigned int j = 0; j < N; ++j)
			{
				// TODO : parallel for?
				m_heightMapCPU[0][i][j] = (m_heightMapCPU[0][i][j] + m_heightMapCPU[1][i][j] + m_heightMapCPU[2][i][j] + m_heightMapCPU[3][i][j]) / CASCADE_COUNT;
			}
		}
	}



	// TODO : run Foam Simulation on height map(Result IFFT Texture), get Turbulence Map using Jacobian and displacement
}

void Ocean::Draw(ID3D11DeviceContext1* context)
{
	// Set vertex shader <- resources : Height map, displacement map
	// m_oceanPlane.Draw(context);
	// Set Pixel Shader <- resources : Normal map, Jacobian Map, PBR constants
}
