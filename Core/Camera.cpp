#include "pch.h"
#include "Camera.h"

const DirectX::XMVECTORF32 Camera::START_POSITION = { 0.f, -1.5f, 0.f, 0.f };
const float Camera::ROTATION_GAIN = 0.004f;
const float Camera::MOVEMENT_GAIN = 0.07f;

using namespace DirectX::SimpleMath;

Camera::Camera(Vector3 eyePosWorld)
	: SSceneObject()
{
	m_posWorld = eyePosWorld;
}

Quaternion Camera::GetPitchYawInQuarternion() const
{
	return Quaternion::CreateFromYawPitchRoll(m_yawEuler, m_pitchEuler, m_rollEuler);
}

void Camera::Reset()
{
	m_posWorld = Camera::START_POSITION;
	m_pitchEuler = 0;
	m_yawEuler = 0;
}