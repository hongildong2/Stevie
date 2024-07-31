 #pragma once
class Camera
{
public:
	Camera() = default;
	Camera(DirectX::SimpleMath::Vector3 eyePos, DirectX::SimpleMath::Vector3 viewDir, DirectX::SimpleMath::Vector3 upVector);
	DirectX::SimpleMath::Matrix GetViewMatrix() const;
	DirectX::SimpleMath::Vector3 GetEyePos() const;
	DirectX::SimpleMath::Vector3 GetEyeDir() const;

	void UpdatePitchYaw(DirectX::SimpleMath::Vector3& deltaRadian)
	{
		m_pitch -= deltaRadian.y;
		m_yaw -= deltaRadian.x;
	}

	void UpdateLookAt(DirectX::SimpleMath::Vector3& viewDir);
	void UpdatePos(DirectX::SimpleMath::Vector3& deltaPos);
	void Reset();

	DirectX::SimpleMath::Quaternion GetPitchYawInQuarternion() const
	{
		return DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(m_yaw, m_pitch, 0.f);
	}

	const static DirectX::XMVECTORF32 START_POSITION;
	const static float ROTATION_GAIN;
	const static float MOVEMENT_GAIN;
	// Rotate? Move?

private:
	DirectX::SimpleMath::Vector3 m_EyePosWorld;
	DirectX::SimpleMath::Vector3 m_LookAtTargetPosWorld;
	DirectX::SimpleMath::Vector3 m_UpVector;

	float m_pitch;
	float m_yaw;
};

