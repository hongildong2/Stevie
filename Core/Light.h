#pragma once

#include "pch.h"
#include "Core/SSceneObject.h"

enum class ELightType
{
	DIRECTIONAL = 1,
	POINT = 2,
	SPOT = 3
};

struct LightData
{
	Vector3 radiance{};
	FLOAT fallOffStart = 0.f;

	Vector3 direction{};
	FLOAT fallOffEnd = 0.f;

	Vector3 positionWorld{};
	FLOAT spotPower = 0.f;

	Vector3 color{};
	BOOL bIsShadowing = FALSE;

	ELightType type = ELightType::DIRECTIONAL;
	FLOAT radius = 0.f;
	FLOAT haloRadius = 0.f;
	FLOAT haloStrength = 0.f;
};
static_assert(sizeof(LightData) % 16 == 0, "CONSTANT BUFFER ALIGNMENT");



class Light final : public SSceneObject
{
public:
	Light(const ELightType type, const Vector3 direction, const Vector3 posWorld, const BOOL bIsShadowing);
	~Light() = default;

	void GetLightData(LightData* outLightData) const;
	void UpdateLightData(LightData& data);

	inline BOOL IsShadowingLight() const
	{
		return m_bIsShadowingLight;
	}

private:
	ELightType m_type;
	Vector3 m_radiance;
	Vector3 m_color;

	float m_fallOffStart;
	float m_fallOffEnd;
	float m_spotPower;

	float m_radius;
	float m_haloRadius;
	float m_haloStrength;

	BOOL m_bIsShadowingLight;
};

