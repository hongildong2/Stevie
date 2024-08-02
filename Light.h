#pragma once
#include "pch.h"
#include "IDepthRenderable.h"

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


class Light final : public IDepthRenderable
{
public:
	Light(const ELightType type, const 	DirectX::SimpleMath::Vector3 direction, const DirectX::SimpleMath::Vector3 posWorld);
	~Light() = default;

	void GetLightData(LightData* outLightData) const;
	DirectX::SimpleMath::Matrix GetViewRow() const override;

	inline ID3D11ShaderResourceView* GetShadowMapSRV() const
	{
		return IDepthRenderable::m_depthTex->GetSRV();
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
};

