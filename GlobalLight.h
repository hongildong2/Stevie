#pragma once

#include <vector>
#include "SimpleMath.h"

enum class ELightType
{
	DIRECTIONAL = 1,
	POINT = 2,
	SPOT = 3,
	SUN = 4
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

	inline ID3D11ShaderResourceView* GetLightSRV() const
	{
		return m_lightsSRV.Get();
	}

	inline const UINT GetLightsCount() const
	{
		return static_cast<UINT>(m_lights.size());
	}

private:
	std::vector<LightData> m_lights;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_lightsSB;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_lightsSRV;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_shadowMaps; // perLight
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shadowMapsSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_shadowMapsDSV;

	D3D11_VIEWPORT m_shadowViewport;
	DirectX::SimpleMath::Matrix m_proj;
};

