#include "pch.h"

#include "SubModules\Render\Scene\SceneStateObject.h"
#include "Light.h"
#include "AObject.h"

Light::Light(const char* name, const ELightType type, const DirectX::SimpleMath::Vector3 direction, const DirectX::SimpleMath::Vector3 posWorld)
	: AObject(name, EObjectType::LIGHT)
	, IGUIComponent(EGUIType::LIGHT)
	, m_type(type)
	, m_direction(direction)
	, m_positionWorld(posWorld)
	, m_radiance{ 2.f, 2.f, 2.f }
	, m_color{ 255.f / 255.f, 238.f / 255.f, 80.f / 255.f }
	, m_fallOffStart(0.f)
	, m_fallOffEnd(20.f)
	, m_spotPower(6.f)
	, m_radius(0.03f)
	, m_haloRadius(0.01f)
	, m_haloStrength(1.f)
{
	AObject::SetComponentFlag(EComponentsFlag::GUI);
	IDepthRenderable::m_proj = type == ELightType::DIRECTIONAL ?
		DirectX::SimpleMath::Matrix::CreateOrthographic(10.f, 10.f, SceneStateObject::NEAR_Z, SceneStateObject::FAR_Z)
		:
		DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(DirectX::XMConvertToRadians(120.f), 1, SceneStateObject::NEAR_Z, SceneStateObject::FAR_Z);
	D3D11_VIEWPORT vp{};
	vp.Width = SceneStateObject::SHADOW_MAP_SIZE;
	vp.Height = SceneStateObject::SHADOW_MAP_SIZE;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;

	m_depthTex = std::make_unique<DepthTexture>(vp);
}


AObject* Light::GetThis()
{
	return this;
}

void Light::GetLightData(LightData* outLightData) const
{
	outLightData->radiance = m_radiance;
	outLightData->fallOffStart = m_fallOffStart;
	outLightData->direction = m_direction;
	outLightData->fallOffEnd = m_fallOffEnd;
	outLightData->positionWorld = m_positionWorld;
	outLightData->spotPower = m_spotPower;

	outLightData->color = m_color;
	outLightData->type = m_type;
	outLightData->radius = m_radius;
	outLightData->haloRadius = m_haloRadius;
	outLightData->haloStrength = m_haloStrength;

	auto view = GetViewRow();
	auto proj = GetProjRow();
	outLightData->viewColumn = view.Transpose();
	outLightData->projColumn = proj.Transpose();

	auto invProj = proj.Invert();
	outLightData->invProjColumn = invProj.Transpose();
}

void Light::UpdateLightData(LightData& data)
{
	m_radiance = data.radiance;
	m_fallOffEnd = data.fallOffEnd;
	m_fallOffStart = data.fallOffStart;

	m_direction = data.direction;
	m_spotPower = data.spotPower;

	m_color = data.color;
	m_radius = data.radius;
	m_haloRadius = data.haloRadius;
	m_haloStrength = data.haloStrength;
}
void Light::UpdatePosWorld(const DirectX::SimpleMath::Vector3& posWorld)
{
	m_positionWorld = posWorld;
}
DirectX::SimpleMath::Matrix Light::GetViewRow() const
{
	DirectX::SimpleMath::Vector3 up = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
	if (std::abs(up.Dot(m_direction) + 1.0f) < 1e-5)
	{
		up = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);

	}

	// TODO : 섀도우맵이 렌더링하는 뷰 프러스트럼을 전부 커버할 수 있도록 Directional light의 position을 camera의 position에 따라 변경
	return DirectX::SimpleMath::Matrix::CreateLookAt(m_positionWorld, m_positionWorld + m_direction, up);
}