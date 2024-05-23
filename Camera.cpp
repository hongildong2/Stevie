#include "pch.h"
#include "Camera.h"



const DirectX::XMVECTORF32 Camera::START_POSITION = { 0.f, -1.5f, 0.f, 0.f };
const float Camera::ROTATION_GAIN = 0.004f;
const float Camera::MOVEMENT_GAIN = 0.07f;



Camera::Camera(DirectX::SimpleMath::Vector3 eyePos, DirectX::SimpleMath::Vector3 viewDir, DirectX::SimpleMath::Vector3 upVector) : m_EyePos(eyePos), m_ViewDir(viewDir), m_UpVector(upVector)
{

}
DirectX::SimpleMath::Matrix Camera::GetViewMatrix() const
{
	return DirectX::SimpleMath::Matrix::CreateLookAt(m_EyePos, m_ViewDir, m_UpVector);
}

DirectX::SimpleMath::Vector3 Camera::GetEyePos() const
{
	return m_EyePos;
}

void Camera::UpdateLookAtBy(DirectX::SimpleMath::Vector3 viewDir)
{
	m_ViewDir = m_EyePos + viewDir;
}

void Camera::UpdatePosBy(DirectX::SimpleMath::Vector3 pos)
{
	m_EyePos += pos;
}

void Camera::Reset()
{
	m_EyePos = Camera::START_POSITION;

}