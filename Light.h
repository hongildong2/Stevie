#pragma once
#include "pch.h"
#include "IDepthRenderable.h"
#include "AObject.h"
#include "IGUIComponent.h"

enum class ELightType
{
	DIRECTIONAL = 1,
	POINT = 2,
	SPOT = 3
};

struct LightData
{
	DirectX::SimpleMath::Vector3 radiance;
	float fallOffStart;

	DirectX::SimpleMath::Vector3 direction;
	float fallOffEnd;

	DirectX::SimpleMath::Vector3 positionWorld;
	float spotPower;

	DirectX::SimpleMath::Vector3 color;
	float dummy;

	ELightType type;
	float radius;
	float haloRadius;
	float haloStrength;

	DirectX::SimpleMath::Matrix viewColumn;
	DirectX::SimpleMath::Matrix projColumn;
	DirectX::SimpleMath::Matrix invProjColumn;
};


class Light final : public AObject, public IGUIComponent, public IDepthRenderable
{
public:
	Light(const char* name, const ELightType type, const DirectX::SimpleMath::Vector3 direction, const DirectX::SimpleMath::Vector3 posWorld);
	~Light() = default;

	AObject* GetThis() override;

	void GetLightData(LightData* outLightData) const;
	void UpdateLightData(LightData& data);
	void UpdatePosWorld(const DirectX::SimpleMath::Vector3& posWorld);
	DirectX::SimpleMath::Matrix GetViewRow() const override;

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
};

