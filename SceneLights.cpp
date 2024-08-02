#include "pch.h"

#include "SceneLights.h"
#include "Utility.h"
#include "DirectXMath.h"

SceneLights::SceneLights(float nearZ, float farZ)
	: IDepthRenderable()
	, m_nearZ(nearZ)
	, m_farZ(farZ)
	, m_lightIndex(0)
{
}
void SceneLights::AddLight(const LightData& lightData)
{
	if (m_lightsSB != nullptr) // buffer already initialized, sorry
	{
		return;
	}

	m_lights.push_back(lightData);
}

void SceneLights::SetNextLight()
{
	m_lightIndex = (m_lightIndex + 1) % GetLightsCount();
}

void SceneLights::Initialize(ID3D11Device1* pDevice)
{
	const unsigned int LIGHTS_COUNT = GetLightsCount();

	Utility::DXResource::CreateStructuredBuffer(pDevice, sizeof(LightData), LIGHTS_COUNT, m_lights.data(), m_lightsSB.GetAddressOf());
	Utility::DXResource::CreateStructuredBufferSRV(pDevice, m_lightsSB.Get(), 0, m_lightsSRV.GetAddressOf());

	D3D11_VIEWPORT shadowVP = { NULL, };
	shadowVP.TopLeftX = 0;
	shadowVP.TopLeftY = 0;
	shadowVP.Width = 1024;
	shadowVP.Height = 1024;
	shadowVP.MinDepth = 0.0f;
	shadowVP.MaxDepth = 1.0f;

	for (unsigned int i = 0; i < LIGHTS_COUNT; ++i)
	{
		std::unique_ptr<DepthTexture> lightShadowMap = std::make_unique<DepthTexture>(shadowVP);
		m_depthTextures.push_back(std::move(lightShadowMap));
	}
}

DepthOnlyConstant SceneLights::GetDepthOnlyConstant() const
{
	DepthOnlyConstant uuu =
	{
		m_lights[m_lightIndex].view,
		m_lights[m_lightIndex].proj
	};

	return uuu;
}

void SceneLights::SetContextDepthOnly(ID3D11DeviceContext1* pContext)
{
	// set resourcs to current light
	// 시작할때 라이트인덱스 0이어야하고, 라이트 수만큼만 불러야함. 지금만..
	unsigned int prevLightIndex = GetCurrentLightIndex() - 1;

	m_depthTex.swap(m_depthTextures[m_lightIndex]);
}

void SceneLights::Update(ID3D11DeviceContext1* pContext)
{
	const float ORTHO_SIZE = 10.f; // defined in world space unit
	const float FOV_IN_ANGLE = 120.f;

	for (auto& l : m_lights)
	{
		DirectX::SimpleMath::Vector3 up = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
		if (std::abs(up.Dot(l.direction) + 1.0f) < 1e-5)
		{
			up = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);

		}

		// TODO : 섀도우맵이 렌더링하는 뷰 프러스트럼을 전부 커버할 수 있도록 Directional light의 position을 camera의 position에 따라 변경
		auto view = DirectX::SimpleMath::Matrix::CreateLookAt(l.positionWorld, l.positionWorld + l.direction, up);

		// different with viewport, proj matrix coord unit is defined in world space
		auto proj = l.type == ELightType::DIRECTIONAL ?
			DirectX::SimpleMath::Matrix::CreateOrthographic(10.f, 10.f, m_nearZ, m_farZ)
			:
			DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(DirectX::XMConvertToRadians(FOV_IN_ANGLE), 1, m_nearZ, m_farZ);

		l.proj = proj.Transpose();
		l.view = view.Transpose();
		l.invProj = proj.Invert().Transpose();
	}

	Utility::DXResource::UpdateBuffer(pContext, m_lightsSB.Get(), sizeof(LightData), static_cast<UINT>(m_lights.size()), m_lights.data());
}