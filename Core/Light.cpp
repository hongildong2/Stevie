#include "pch.h"
#include "Light.h"

Light::Light(const ELightType type)
	: SSceneObject()
	, m_type(type)
	, m_radiance(1.f)
	, m_color{ 1.f, 1.f, 1.f }
	, m_fallOffStart(1.f)
	, m_fallOffEnd(20.f)
	, m_spotPower(1.f)
	, m_radius(1.f)
	, m_bIsShadowingLight(FALSE)
{
}

void Light::SetRadiance(const FLOAT radiance)
{
	m_radiance = radiance;
}

void Light::SetLightType(const ELightType type)
{
	m_type = type;
}

void Light::SetShadowing(const BOOL bIsShadowing)
{
	m_bIsShadowingLight = bIsShadowing;
}

void Light::SetLightRadius(const FLOAT radius)
{
	MY_ASSERT(radius >= 0.01f);
	m_radius = radius;
}


RLightConstant&& Light::GetLightConstant() const
{
	return RLightConstant(m_type, GetDirWorld(), GetPosWorld(), m_color, m_radiance, m_fallOffStart, m_fallOffEnd, m_spotPower, m_radius);
}

