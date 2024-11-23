 #pragma once

#include "pch.h"
#include "Core/SSceneObject.h"

struct RLightConstant;

class Light final : public SSceneObject
{
public:
	Light(const ELightType type);
	~Light() = default;

	void SetLightType(const ELightType type);
	void SetShadowing(const BOOL bIsShadowing);
	void SetLightRadius(const FLOAT radius);
	RLightConstant&& GetLightConstant() const;

	inline BOOL IsShadowingLight() const
	{
		return m_bIsShadowingLight;

	}

private:
	ELightType m_type;
	FLOAT m_radiance;
	Vector3 m_color;

	float m_fallOffStart;
	float m_fallOffEnd;
	float m_spotPower;
	float m_radius;

	BOOL m_bIsShadowingLight;
};

