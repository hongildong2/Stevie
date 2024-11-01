#include "pch.h"
#include "Camera.h"

const DirectX::XMVECTORF32 Camera::START_POSITION = { 0.f, -1.5f, 0.f, 0.f };
const float Camera::ROTATION_GAIN = 0.004f;
const float Camera::MOVEMENT_GAIN = 0.07f;

using namespace DirectX::SimpleMath;

Camera::Camera(Vector3 eyePosWorld, float aspectRatio, float nearZ, float farZ, float fov)
	: SSceneObject()
	, m_aspectRatio(aspectRatio)
	, m_nearZ(nearZ)
	, m_farZ(farZ)
	, m_fov(fov)
{
	m_posWorld = eyePosWorld;
}


Matrix Camera::GetProjRowMat() const
{
	return Matrix::CreatePerspectiveFieldOfView(m_fov, m_aspectRatio, m_nearZ, m_farZ);
}

Quaternion Camera::GetPitchYawInQuarternion() const
{
	return Quaternion::CreateFromYawPitchRoll(m_yawEuler, m_pitchEuler, m_rollEuler);
}

void Camera::UpdateAspectRatio(float aspectRatio)
{
	m_aspectRatio = aspectRatio;
}

void Camera::Reset()
{
	m_posWorld = Camera::START_POSITION;
	m_pitchEuler = 0;
	m_yawEuler = 0;
}