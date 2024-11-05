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
	DirectX::SimpleMath::Vector3 radiance;
	FLOAT fallOffStart;

	DirectX::SimpleMath::Vector3 direction;
	FLOAT fallOffEnd;

	DirectX::SimpleMath::Vector3 positionWorld;
	FLOAT spotPower;

	DirectX::SimpleMath::Vector3 color;
	BOOL bIsShadowing;

	ELightType type;
	FLOAT radius;
	FLOAT haloRadius;
	FLOAT haloStrength;
};
static_assert(sizeof(LightData) % 16 == 0, "CONSTANT BUFFER ALIGNMENT");



class Light final : public SSceneObject
{
public:
	Light(const ELightType type, const DirectX::SimpleMath::Vector3 direction, const DirectX::SimpleMath::Vector3 posWorld, const BOOL bIsShadowing);
	~Light() = default;

	void GetLightData(LightData* outLightData) const;
	void UpdateLightData(LightData& data);

	inline BOOL IsShadowingLight() const
	{
		return m_bIsShadowingLight;
	}

private:
	ELightType m_type;
	DirectX::SimpleMath::Vector3 m_radiance;
	DirectX::SimpleMath::Vector3 m_color;

	float m_fallOffStart;
	float m_fallOffEnd;
	float m_spotPower;

	float m_radius;
	float m_haloRadius;
	float m_haloStrength;

	BOOL m_bIsShadowingLight;
};

