#pragma once
#include "pch.h"
#include "SimpleMath.h"
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

	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix proj;
	DirectX::SimpleMath::Matrix invProj;
};


class SceneLights final : public IDepthRenderable
{
public:
	SceneLights(float nearZ, float farZ);
	virtual ~SceneLights() = default;

	SceneLights(const SceneLights& other) = delete;
	SceneLights& operator=(const SceneLights& other) = delete;

	void Update(ID3D11DeviceContext1* pContext);
	void Initialize(ID3D11Device1* pDevice);
	void AddLight(const LightData& lightData);

	// 개망했다. Light 클래스 IDepthRenderable로 분리해야한다.
	// Structured Buffer는 .. 라이트클래스에서 값복사로 받아와서 여기서 업데이트 하는걸로
	// Light 관리 클래스로 바꾸자...
	virtual DepthOnlyConstant GetDepthOnlyConstant() const override;
	virtual void SetContextDepthOnly(ID3D11DeviceContext1* pContext) override;

	inline const unsigned int GetCurrentLightIndex() const
	{
		return m_lightIndex;
	}

	void SetNextLight();

	// mutable
	inline const std::vector<LightData>& GetLights()
	{
		return m_lights;
	}

	// 이런식으로 하는게 맞을까? 요걸 사용하는 클래스에서 조작을 하는게 맞는것같은데..
	// 말이안됨 그냥
	inline ID3D11ShaderResourceView* GetLightsSRV() const
	{
		return m_lightsSRV.Get();
	}

	inline const unsigned int GetLightsCount() const
	{
		return static_cast<unsigned int>(m_lights.size());
	}

private:
	std::vector<LightData> m_lights;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_lightsSB;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_lightsSRV;

	std::vector<std::unique_ptr<DepthTexture>> m_depthTextures;

	const float m_nearZ;
	const float m_farZ;
	unsigned int m_lightIndex;
};

