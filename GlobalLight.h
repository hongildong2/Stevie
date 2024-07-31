#pragma once

#include <vector>
#include "SimpleMath.h"

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

	DirectX::SimpleMath::Matrix proj;
	DirectX::SimpleMath::Matrix invProj;
	DirectX::SimpleMath::Matrix viewProj; // light's view
};


class SceneLights final
{
public:
	SceneLights(float shadowMapSize, float nearZ, float farZ);
	~SceneLights() = default;

	SceneLights(const SceneLights& other) = default;
	SceneLights& operator=(const SceneLights& other) = default;

	void Update(ID3D11DeviceContext1* pContext);

	void Initialize(ID3D11Device1* pDevice);

	void AddLight(const LightData& lightData);

	inline const D3D11_VIEWPORT* GetShadowViewport() const
	{
		return &m_shadowViewport;
	}

	// mutable
	inline const std::vector<LightData>& GetLights()
	{
		return m_lights;
	}

	// 이런식으로 하는게 맞을까? 요걸 사용하는 클래스에서 조작을 하는게 맞는것같은데..
	inline ID3D11ShaderResourceView* GetLightSRV() const
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

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_shadowMaps; // perLight
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shadowMapsSRV;
	std::vector<Microsoft::WRL::ComPtr<ID3D11DepthStencilView>> m_shadowMapsDSVs;

	D3D11_VIEWPORT m_shadowViewport;
	const float m_nearZ;
	const float m_farZ;
};

