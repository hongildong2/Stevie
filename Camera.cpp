#include "pch.h"
#include "Camera.h"



const DirectX::XMVECTORF32 Camera::START_POSITION = { 0.f, -1.5f, 0.f, 0.f };
const float Camera::ROTATION_GAIN = 0.004f;
const float Camera::MOVEMENT_GAIN = 0.07f;



Camera::Camera(DirectX::SimpleMath::Vector3 eyePosWorld, DirectX::SimpleMath::Vector3 viewDirWorld, DirectX::SimpleMath::Vector3 upVector, float nearZ, float farZ, float fov)
	: IDepthRenderable()
	, m_eyePosWorld(eyePosWorld)
	, m_lookAtTargetPosWorld(eyePosWorld + viewDirWorld)
	, m_upVector(upVector)
	, m_pitch(0.f)
	, m_yaw(0.f)
	, m_nearZ(nearZ)
	, m_farZ(farZ)
	, m_fov(fov)
{
}

DirectX::SimpleMath::Matrix Camera::GetViewMatrix() const
{
	return DirectX::SimpleMath::Matrix::CreateLookAt(m_eyePosWorld, m_lookAtTargetPosWorld, m_upVector);
}

DirectX::SimpleMath::Matrix Camera::GetProjMatrix() const
{
	return m_proj;
}

DirectX::SimpleMath::Vector3 Camera::GetEyePos() const
{
	return m_eyePosWorld;
}

DirectX::SimpleMath::Vector3 Camera::GetEyeDir() const
{
	DirectX::SimpleMath::Vector3 viewDir = m_lookAtTargetPosWorld - m_eyePosWorld;
	viewDir.Normalize();
	return viewDir;
}

// LookAt은 viewDir가 아니라, 바라보는 Point이다. eyePosWorld + viewDir = LookAt
// LookAt = eyePosWorld + viewDir
// viewDir은 Model 좌표계 기준 camera model == world
void Camera::UpdateLookAt(DirectX::SimpleMath::Vector3& viewDirModel)
{
	m_lookAtTargetPosWorld = m_eyePosWorld + viewDirModel;
}

void Camera::UpdatePos(DirectX::SimpleMath::Vector3& deltaPos)
{
	m_eyePosWorld += deltaPos;

	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	constexpr float limit = XM_PIDIV2 - 0.01f;
	m_pitch = std::max(-limit, m_pitch);
	m_pitch = std::min(+limit, m_pitch);

	// keep longitude in sane range by wrapping
	if (m_yaw > XM_PI)
	{
		m_yaw -= XM_2PI;
	}
	else if (m_yaw < -XM_PI)
	{
		m_yaw += XM_2PI;
	}

	/*
		initially, camera's model space is aligned with world space because we defined it in same coordinate system
		camera's view dir is represented in camera's model space`s unit sphere
		using pitch yaw as angles
	*/
	// defined by angles in unit sphere
	float y = sinf(m_pitch);
	float r = cosf(m_pitch);
	float z = r * cosf(m_yaw);
	float x = r * sinf(m_yaw);
	auto camearaViewDirModel = Vector3(x, y, z);
	UpdateLookAt(camearaViewDirModel);
}

DepthOnlyConstant Camera::GetDepthOnlyConstant() const
{
	DepthOnlyConstant disaster =
	{
		GetViewMatrix(),
		GetProjMatrix()
	};

	return disaster;
}

void Camera::OnWindowSizeChange(ID3D11Device1* pDevice, D3D11_VIEWPORT vp, DXGI_FORMAT bufferFormat)
{
	m_proj = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(DirectX::XMConvertToRadians(m_fov), float(vp.Width) / float(vp.Height), m_nearZ, m_farZ);
	m_depthTex.reset();
	m_depthTex = std::make_unique<DepthTexture>(vp);
}

void Camera::Reset()
{
	m_eyePosWorld = Camera::START_POSITION;
	m_pitch = 0;
	m_yaw = 0;
}