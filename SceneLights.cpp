#include "pch.h"

#include "SceneLights.h"
#include "Utility.h"
#include "DirectXMath.h"

SceneLights::SceneLights()
	: m_lightDataBuffer()
	, m_shadowMapSRVs{ nullptr, }
{
}


void SceneLights::AddLight(std::unique_ptr<Light> light)
{
	if (m_lightsSB != nullptr || GetLightsCount() >= MAX_LIGHT_COUNT) // buffer already initialized, sorry
	{
		return;
	}

	m_lights.push_back(std::move(light));
}

ID3D11ShaderResourceView* const* SceneLights::GetShadowMapSRVs() const
{
	return m_shadowMapSRVs;
}

void SceneLights::Initialize(ID3D11Device1* pDevice)
{
	const unsigned int LIGHTS_COUNT = GetLightsCount();
	ZeroMemory(&m_lightDataBuffer, sizeof(LightData) * MAX_LIGHT_COUNT);

	for (unsigned int i = 0; i < LIGHTS_COUNT; ++i)
	{
		m_lights[i]->Initialize(pDevice);
		m_shadowMapSRVs[i] = m_lights[i]->GetDepthBufferSRV();
	}
	Utility::DXResource::CreateStructuredBuffer(pDevice, sizeof(LightData), LIGHTS_COUNT, m_lightDataBuffer, m_lightsSB.GetAddressOf());
	Utility::DXResource::CreateStructuredBufferSRV(pDevice, m_lightsSB.Get(), 0, m_lightsSBSRV.GetAddressOf());
}

void SceneLights::Update(ID3D11DeviceContext1* pContext)
{
	const unsigned int LIGHTS_COUNT = GetLightsCount();
	for (unsigned int i = 0; i < LIGHTS_COUNT; ++i)
	{
		auto& lightRef = m_lights[i];
		LightData* bufferPtr = m_lightDataBuffer + i;

		lightRef->GetLightData(bufferPtr);
	}

	Utility::DXResource::UpdateBuffer(pContext, m_lightsSB.Get(), sizeof(LightData), LIGHTS_COUNT, m_lightDataBuffer);
}