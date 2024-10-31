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

	DirectX::SimpleMath::Matrix viewColumn;
	DirectX::SimpleMath::Matrix projColumn;
	DirectX::SimpleMath::Matrix invProjColumn;
};
static_assert(sizeof(LightData) % 16 == 0, "CONSTANT BUFFER ALIGNMENT");



class Light final : public SSceneObject
{
public:
	Light(const ELightType type, const DirectX::SimpleMath::Vector3 direction, const DirectX::SimpleMath::Vector3 posWorld);
	~Light() = default;


	void GetLightData(LightData* outLightData) const;
	void UpdateLightData(LightData& data);
	void UpdatePosWorld(const DirectX::SimpleMath::Vector3& posWorld);

	DirectX::SimpleMath::Matrix GetViewRow() const;
	DirectX::SimpleMath::Matrix GetProjRow() const
	{
		return m_proj;
	}


private:
	DirectX::SimpleMath::Vector3 m_radiance;
	DirectX::SimpleMath::Vector3 m_direction;
	DirectX::SimpleMath::Vector3 m_positionWorld;
	DirectX::SimpleMath::Vector3 m_color;

	float m_fallOffStart;
	float m_fallOffEnd;
	float m_spotPower;

	ELightType m_type;
	float m_radius;
	float m_haloRadius;
	float m_haloStrength;

	BOOL m_bIsShadowingLight;

	DirectX::SimpleMath::Matrix m_proj;
};

