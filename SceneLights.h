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

	// �����ߴ�. Light Ŭ���� IDepthRenderable�� �и��ؾ��Ѵ�.
	// Structured Buffer�� .. ����ƮŬ�������� ������� �޾ƿͼ� ���⼭ ������Ʈ �ϴ°ɷ�
	// Light ���� Ŭ������ �ٲ���...
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

	// �̷������� �ϴ°� ������? ��� ����ϴ� Ŭ�������� ������ �ϴ°� �´°Ͱ�����..
	// ���̾ȵ� �׳�
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

