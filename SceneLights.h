#pragma once
#include "pch.h"
#include "SimpleMath.h"
#include "IDepthRenderable.h"
#include "Light.h"


class SceneLights final
{
public:
	SceneLights();
	~SceneLights() = default;

	SceneLights(const SceneLights& other) = delete;
	SceneLights& operator=(const SceneLights& other) = delete;

	void Update(ID3D11DeviceContext1* pContext);
	void Initialize(ID3D11Device1* pDevice);
	void AddLight(std::unique_ptr<Light> light);
	ID3D11ShaderResourceView* const* GetShadowMapSRVs() const;

	inline ID3D11ShaderResourceView* GetLightsSBSRV() const
	{
		return m_lightsSBSRV.Get();
	}

	inline const unsigned int GetLightsCount() const
	{
		return static_cast<unsigned int>(m_lights.size());
	}


	enum { MAX_LIGHT_COUNT = 19 };
private:
	std::vector<std::unique_ptr<Light>> m_lights;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_lightsSB;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_lightsSBSRV;

	LightData m_lightDataBuffer[MAX_LIGHT_COUNT];
	ID3D11ShaderResourceView* m_shadowMapSRVs[MAX_LIGHT_COUNT];
};

