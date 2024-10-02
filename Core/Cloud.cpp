#include "pch.h"
#include "Cloud.h"


constexpr unsigned int CLOUD_TEX_SIZE = 512;

Cloud::Cloud(const unsigned int cloudCounts)
	: Model("CLOUD", EModelType::DEFAULT, Graphics::cloudPSO)
	, m_cloudCount(cloudCounts)
	, m_densityTexture(DXGI_FORMAT_R16_FLOAT, CLOUD_TEX_SIZE, CLOUD_TEX_SIZE, CLOUD_TEX_SIZE)
	, m_lightingTexture(DXGI_FORMAT_R16_FLOAT, CLOUD_TEX_SIZE, CLOUD_TEX_SIZE, CLOUD_TEX_SIZE)
	, mb_initialized(false)
{
	IGUIComponent::m_type = EGUIType::MODEL;
}

void Cloud::Initialize(ID3D11Device1* pDevice)
{
	m_densityTexture.SetDevice(pDevice);
	m_lightingTexture.SetDevice(pDevice);

	m_densityTexture.Initialize();
	m_lightingTexture.Initialize();



	auto a = GeometryGenerator::MakeBox(1.f);
	UpdatePosByCoordinate({ 0.f, 2.f, 0.f, 1.f });
	DirectX::SimpleMath::Vector3 relPos(0.f, 0.f, 0.f);

	auto pickRandom = [&]()
		{
			relPos.x = rand() % 100 - 50;
			relPos.z = rand() % 100 - 50;
		};

	for (unsigned int i = 0; i < m_cloudCount; ++i)
	{
		std::unique_ptr<MeshPart> box = std::make_unique<MeshPart>(a, EMeshType::SOLID, pDevice);
		box->UpdateRelativePos(relPos);

		AddMeshComponent(std::move(box));

		pickRandom();
	}

	Model::Initialize(pDevice);
}

void Cloud::InitializeData(ID3D11DeviceContext1* pContext)
{
	ID3D11UnorderedAccessView* uavs[2] = { m_densityTexture.GetUAV(), m_lightingTexture.GetUAV() };
	ID3D11ShaderResourceView* srvs[1] = { m_densityTexture.GetSRV() };
	ID3D11SamplerState* ss[1] = { Graphics::linearClampSS.Get() };

	// generate noise texture to density
	Graphics::SetPipelineState(pContext, Graphics::cloudDensityPSO);
	pContext->CSSetSamplers(0, 1, ss);
	pContext->CSSetUnorderedAccessViews(0, 1, uavs, nullptr);
	pContext->Dispatch(CLOUD_TEX_SIZE / 16, CLOUD_TEX_SIZE / 16, CLOUD_TEX_SIZE / 4);

	Utility::ComputeShaderBarrier(pContext);

	// generate lighting texture from density texture
	Graphics::SetPipelineState(pContext, Graphics::cloudLightingPSO);
	pContext->CSSetUnorderedAccessViews(0, 1, uavs + 1, nullptr);
	pContext->CSSetShaderResources(60, 1, srvs);
	pContext->Dispatch(CLOUD_TEX_SIZE / 16, CLOUD_TEX_SIZE / 16, CLOUD_TEX_SIZE / 4);
	Utility::ComputeShaderBarrier(pContext);

	srvs[0] = nullptr;
	pContext->CSSetShaderResources(60, 1, srvs);
	mb_initialized = true;
}



void Cloud::Update(ID3D11DeviceContext1* pContext)
{
	if (!mb_initialized)
	{
		InitializeData(pContext);

		return;
	}

	Model::Update(pContext);
}

void Cloud::Render(ID3D11DeviceContext1* pContext)
{
	ID3D11ShaderResourceView* srvs[2] = { m_densityTexture.GetSRV(), m_lightingTexture.GetSRV() };
	pContext->PSSetShaderResources(60, 2, srvs);

	Model::Render(pContext);
}
