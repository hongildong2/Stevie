#include "pch.h"
#include "Ocean.h"
#include "GraphicsCommon.h"

Ocean::Ocean(ID3D11Device1* device)
	:mb_initialized(false),
	m_oceanPlane("OceanPlane", { ModelMeshPart(GeometryGenerator::MakeSquare(), device) }, { 0.f, 0.f, 0.f })
{
	// create d3d resources
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	// TODO : Check CPU Readonly acceptable description
	desc.Width = N;
	desc.Height = N;
	desc.MipLevels = 1;
	desc.ArraySize = CASCADE_COUNT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	// Stating HeightMap Texture for CPU
	device->CreateTexture2D(&desc, NULL, m_heightMap.GetAddressOf());


	desc.Usage = D3D11_USAGE_DEFAULT;
	// Spectrum Map Texture
	device->CreateTexture2D(&desc, NULL, m_spectrumMap.GetAddressOf());


	// Wave vector datas
	device->CreateTexture2D(&desc, NULL, m_waveVectorData.GetAddressOf());

	// Initial Spectrum Textures, float2 Texture2DArray
	desc.Format = DXGI_FORMAT_R16G16_FLOAT;
	device->CreateTexture2D(&desc, NULL, m_initialSpectrumMap.GetAddressOf());

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


}

void Ocean::InitData(ID3D11DeviceContext1* context)
{
	// run initspectrum CS, get initial spectrum map
	Graphics::SetPipelineState(context, Graphics::Ocean::initialSpectrumPSO);
	// TODO : Set UAV Resources as Texture2DArray
	ID3D11UnorderedAccessView* uavs[] = { m_initialSpectrumMapUAV.Get(), m_waveVectorDataUAV.Get() };
	context->CSSetUnorderedAccessViews(0, 2, uavs, NULL); // TODO : create shader resource view for textures

	context->Dispatch(1, 1, 1);

	// get initial wave vectors

	// TODO : foam simulations

}

void Ocean::Update(ID3D11DeviceContext1* context)
{
	// If wave constant changed, re-run InitData routine
	if (mb_initialized == false)
	{
		InitData(context);
		mb_initialized = true;
	}

	// apply delta time, update spectrum map using tilde h0
	// RUN Time dependent sepctrum CS using textures tilde h0(k,t), and waveData that contains wave vector k
	Graphics::SetPipelineState(context, Graphics::Ocean::timedependentSpectrumPSO);
	// TODO : Set UAV Resources as Texture2DArray
	ID3D11UnorderedAccessView* uavs[] = { m_spectrumMapUAV.Get(), m_initialSpectrumMapUAV.Get(), m_waveVectorDataUAV.Get() };
	context->CSSetUnorderedAccessViews(0, sizeof(uavs), uavs, NULL);
	context->Dispatch(1, 1, 1);
	// save result into FFT texture for IFFT CS to use it 


	// run IFFT CS, renew height map from spectrum map, get normal map from height map
	// use FFT texture updated prev time dependent spectrum CS
	// Run FFT CS twice on FFT texture as it is 2D FFT, one with horizontally and the other one with vertically(order can be changed)

	// horizontally
	Graphics::SetPipelineState(context, Graphics::Ocean::FFTPSO);
	// TODO : Set UAV Resources as Texture2DArray
	ID3D11UnorderedAccessView* uavs[] = { m_spectrumMapUAV.Get() };
	context->CSSetUnorderedAccessViews(0, sizeof(uavs), uavs, NULL);
	context->Dispatch(1, 1, 1);

	// vertically
	Graphics::SetPipelineState(context, Graphics::Ocean::FFTPSO);
	// TODO : Set UAV Resources as Texture2DArray
	ID3D11UnorderedAccessView* uavs[] = { m_spectrumMapUAV.Get() };
	context->CSSetUnorderedAccessViews(0, sizeof(uavs), uavs, NULL);
	context->Dispatch(1, 1, 1);

	// Copy Resource spectrumMap to heightMap Staging Texture


	// TODO : run Foam Simulation on height map(Result IFFT Texture), get Turbulence Map using Jacobian and displacement



}

void Ocean::Draw(ID3D11DeviceContext1* context)
{
	// Set vertex shader <- resources : Height map, displacement map
	m_oceanPlane.Draw(context);
	// Set Pixel Shader <- resources : Normal map, Jacobian Map, PBR constants
}
