#include "pch.h"
#include "SceneStateObject.h"
#include "Light.h"

Light::Light(const ELightType type, const DirectX::SimpleMath::Vector3 direction, const DirectX::SimpleMath::Vector3 posWorld)
	:m_type(type)
	, m_direction(direction)
	, m_positionWorld(posWorld)
	, m_radiance{ 2.f, 2.f, 2.f }
	, m_fallOffStart(0.f)
	, m_fallOffEnd(20.f)
	, m_spotPower(6.f)
	, m_radius(0.02f)
	, m_haloRadius(0.01f)
	, m_haloStrength(1.f)
{
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

	outLightData->viewColumn = GetViewRow().Transpose();
	outLightData->projColumn = GetProjRow().Transpose();
	outLightData->projColumn = outLightData->projColumn.Invert();

}
DirectX::SimpleMath::Matrix Light::GetViewRow() const
{
	DirectX::SimpleMath::Vector3 up = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
	if (std::abs(up.Dot(m_direction) + 1.0f) < 1e-5)
	{
		up = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);

	}

	// TODO : ��������� �������ϴ� �� ������Ʈ���� ���� Ŀ���� �� �ֵ��� Directional light�� position�� camera�� position�� ���� ����
	return DirectX::SimpleMath::Matrix::CreateLookAt(m_positionWorld, m_positionWorld + m_direction, up);
}