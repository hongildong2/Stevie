#pragma once
class Camera
{
public:
	Camera() = default;
	Camera(DirectX::SimpleMath::Vector3 eyePos, DirectX::SimpleMath::Vector3 viewDir, DirectX::SimpleMath::Vector3 upVector);
	DirectX::SimpleMath::Matrix GetViewMatrix() const;
	DirectX::SimpleMath::Vector3 GetEyePos() const;
	void UpdateLookAtBy(DirectX::SimpleMath::Vector3 viewDir);
	void UpdatePosBy(DirectX::SimpleMath::Vector3 pos);
	void Reset();

	const static DirectX::XMVECTORF32 START_POSITION;
	const static float ROTATION_GAIN;
	const static float MOVEMENT_GAIN;
	// Rotate? Move?
private:
	DirectX::SimpleMath::Vector3 m_EyePos;
	DirectX::SimpleMath::Vector3 m_ViewDir;
	DirectX::SimpleMath::Vector3 m_UpVector;
};

