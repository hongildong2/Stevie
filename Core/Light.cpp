#include "pch.h"
#include "Light.h"

Light::Light(const ELightType type, const DirectX::SimpleMath::Vector3 direction, const DirectX::SimpleMath::Vector3 posWorld)
	: m_type(type)
	, m_direction(direction)
	, m_positionWorld(posWorld)
	, m_radiance{ 1.f, 1.f, 1.f }
	, m_color{ 0.f, 1.f, 0.f }
	, m_fallOffStart(0.f)
	, m_fallOffEnd(20.f)
	, m_spotPower(6.f)
	, m_radius(0.03f)
	, m_haloRadius(0.01f)
	, m_haloStrength(1.f)
	, m_bIsShadowingLight(FALSE)
{
	m_proj = type == ELightType::DIRECTIONAL ?
		DirectX::SimpleMath::Matrix::CreateOrthographic(10.f, 10.f, 1.f, 100.f)
		:
		DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(DirectX::XMConvertToRadians(120.f), 1, 1.f, 100.f);
}


void Light::GetLightData(LightData* pOutLightData) const
{
	pOutLightData->radiance = m_radiance;
	pOutLightData->fallOffStart = m_fallOffStart;
	pOutLightData->direction = m_direction;
	pOutLightData->fallOffEnd = m_fallOffEnd;
	pOutLightData->positionWorld = m_positionWorld;
	pOutLightData->spotPower = m_spotPower;

	pOutLightData->color = m_color;
	pOutLightData->type = m_type;
	pOutLightData->radius = m_radius;
	pOutLightData->haloRadius = m_haloRadius;
	pOutLightData->haloStrength = m_haloStrength;

	auto view = GetViewRow();
	auto proj = GetProjRow();
	pOutLightData->viewColumn = view.Transpose();
	pOutLightData->projColumn = proj.Transpose();

	auto invProj = proj.Invert();
	pOutLightData->invProjColumn = invProj.Transpose();
}

void Light::UpdateLightData(LightData& data)
{
	m_radiance = data.radiance;
	m_fallOffEnd = data.fallOffEnd;
	m_fallOffStart = data.fallOffStart;

	m_direction = data.direction;
	m_spotPower = data.spotPower;

	m_color = data.color;
	m_radius = data.radius;
	m_haloRadius = data.haloRadius;
	m_haloStrength = data.haloStrength;
}

void Light::UpdatePosWorld(const DirectX::SimpleMath::Vector3& posWorld)
{
	m_positionWorld = posWorld;
}

DirectX::SimpleMath::Matrix Light::GetViewRow() const
{
	DirectX::SimpleMath::Vector3 up = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
	if (std::abs(up.Dot(m_direction) + 1.0f) < 1e-5)
	{
		up = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);

	}

	return DirectX::SimpleMath::Matrix::CreateLookAt(m_positionWorld, m_positionWorld + m_direction, up);
}


