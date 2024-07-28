#pragma once

#include <vector>
#include "SimpleMath.h"
#include "DirectXMath.h"

struct LightData
{
	DirectX::SimpleMath::Vector3 radiance;
	float fallOffStart;

	DirectX::SimpleMath::Vector3 direction;
	float fallOffEnd;

	DirectX::SimpleMath::Vector3 positionWorld;
	float spotPower;

	UINT type;
	float radius;
	float haloRadius;
	float haloStrength;

	DirectX::SimpleMath::Matrix proj;
	DirectX::SimpleMath::Matrix invProj;
	DirectX::SimpleMath::Matrix viewProj; // light's view
};


class GlobalLight final
{
public:
	GlobalLight(float shadowMapSize, float nearZ, float farZ);
	~GlobalLight() = default;
	GlobalLight(const GlobalLight& other) = default;
	GlobalLight& operator=(const GlobalLight& other) = default;

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

private:
	std::vector<LightData> m_lights;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_lightsSB;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_lightsSRV;

	D3D11_VIEWPORT m_shadowViewport;
	DirectX::SimpleMath::Matrix m_proj;
};

