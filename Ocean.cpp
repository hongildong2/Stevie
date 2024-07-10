#include "pch.h"
#include "Ocean.h"
#include "GraphicsCommon.h"

Ocean::Ocean(ID3D11Device1* device)
	:mb_initialized(false),
	m_oceanPlane("OceanPlane", { ModelMeshPart(GeometryGenerator::MakeSquare(), device) }, {0.f, 0.f, 0.f})
{
	// create d3d resources
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	// TODO : Check CPU Readonly acceptable description
	desc.Width = N;
	desc.Height = N;
	desc.MipLevels = 0;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	// Height Map Textures
	for (auto& el : m_heightMap)
	{
		device->CreateTexture2D(&desc, NULL, el.GetAddressOf());
	}
	// Spectrum Map Textures
	for (auto& el : m_spectrumMap)
	{
		device->CreateTexture2D(&desc, NULL, el.GetAddressOf());
	}

	// Wave vector datas
	for (auto& el : m_waveVectorData)
	{
		device->CreateTexture2D(&desc, NULL, el.GetAddressOf());
	}

	// Initial Spectrum Textures, float2 Texture2DArray
	desc.Format = DXGI_FORMAT_R16G16_FLOAT;
	for (auto& el : m_initialSpectrumMap)
	{
		device->CreateTexture2D(&desc, NULL, el.GetAddressOf());
	}
}

void Ocean::InitData(ID3D11DeviceContext1* context)
{
	// run initspectrum CS, get initial spectrum map
	Graphics::SetPipelineState(context, Graphics::Ocean::initialSpectrumPSO);
	// TODO : Set UAV Resources as Texture2DArray
	ID3D11UnorderedAccessView* uavs[] = {m_initialSpectrumMap->Get(), m_waveVectorData->Get()};
	context->CSSetUnorderedAccessViews(0, 2, uavs); // TODO : create shader resource view for textures

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
	ID3D11UnorderedAccessView* uavs[] = {spectrumMap, initspectrumMap, wavesdata};
	context->Dispatch(1, 1, 1);
	// save result into FFT texture for IFFT CS to use it 


	// run IFFT CS, renew height map from spectrum map, get normal map from height map
	// use FFT texture updated prev time dependent spectrum CS
	// Run FFT CS twice on FFT texture as it is 2D FFT, one with horizontally and the other one with vertically(order can be changed)
	
	// horizontally
	Graphics::SetPipelineState(context, Graphics::Ocean::FFTPSO);
	// TODO : Set UAV Resources as Texture2DArray
	ID3D11UnorderedAccessView* uavs[] = {spectrtumMap};
	context->Dispatch(1,1, 1);

	// vertically
	Graphics::SetPipelineState(context, Graphics::Ocean::FFTPSO);
	// TODO : Set UAV Resources as Texture2DArray
	ID3D11UnorderedAccessView* uavs[] = { spectrtumMap };
	context->Dispatch(1, 1, 1);

	// TODO : run Foam Simulation on height map(Result IFFT Texture), get Turbulence Map using Jacobian and displacement



}

void Ocean::Draw(ID3D11DeviceContext1* context)
{
	// Set vertex shader <- resources : Height map, displacement map
	m_oceanPlane.Draw();
	// Set Pixel Shader <- resources : Normal map, Jacobian Map, PBR constants
}
