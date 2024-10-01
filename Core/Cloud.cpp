#include "pch.h"
#include "Cloud.h"

Cloud::Cloud(const unsigned int cloudCounts)
	: Model("CLOUD", EModelType::DEFAULT, Graphics::basicPSO) // TODO :: cloudPSO
	, m_densityTexture(DXGI_FORMAT_R32G32B32_FLOAT, 512, 512, 512)
	, m_lightingTexture(DXGI_FORMAT_R32G32B32_FLOAT, 512, 512, 512)
	, mb_initialized(false)
{

}

void Cloud::Initialize(ID3D11Device1* pDevice)
{
	m_densityTexture.SetDevice(pDevice);
	m_lightingTexture.SetDevice(pDevice);

	auto a = GeometryGenerator::MakeBox(1.f);
	std::unique_ptr<MeshPart> sph = std::make_unique<MeshPart>(a, EMeshType::SOLID, pDevice, nullptr);
	AddMeshComponent(std::move(sph));

}

void Cloud::InitializeData(ID3D11DeviceContext1* pContext)
{
	// generate noise texture to density

	// generate lighting texture from density texture

	mb_initialized = true;
}



void Cloud::Update(ID3D11DeviceContext1* pContext)
{
	if (!mb_initialized)
	{
		InitializeData(pContext);
		return;
	}



}