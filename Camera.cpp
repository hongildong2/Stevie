#include "pch.h"
#include "Camera.h"


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