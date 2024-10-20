#pragma once
#include "pch.h"

class Camera final
{
public:
	Camera(DirectX::SimpleMath::Vector3 eyePos, DirectX::SimpleMath::Vector3 viewDir, DirectX::SimpleMath::Vector3 upVector, float aspectRatio, float nearZ, float farZ, float fov);
	DirectX::SimpleMath::Vector3 GetEyePos() const;
	DirectX::SimpleMath::Vector3 GetEyeDir() const;
	DirectX::SimpleMath::Matrix GetViewRowMat() const;
	DirectX::SimpleMath::Matrix GetProjRowMat() const;
	inline float GetNearZ() const
	{
		return m_nearZ;
	}
	inline float GetFarZ() const
	{
		return m_farZ;
	}


	void UpdatePitchYaw(DirectX::SimpleMath::Vector3& deltaRadian);
	void UpdateLookAt(DirectX::SimpleMath::Vector3& viewDir);
	void UpdatePos(DirectX::SimpleMath::Vector3& deltaPos);
	void UpdateAspectRatio(float aspectRatio);
	void Reset();
	DirectX::SimpleMath::Quaternion GetPitchYawInQuarternion() const;


private:
	DirectX::SimpleMath::Vector3 m_eyePosWorld;
	DirectX::SimpleMath::Vector3 m_lookAtTargetPosWorld;
	DirectX::SimpleMath::Vector3 m_upVector;

	float m_pitch;
	float m_yaw;

	float m_aspectRatio;
	float m_nearZ;
	float m_farZ;
	float m_fov;

public:
	const static DirectX::XMVECTORF32 START_POSITION;
	const static float ROTATION_GAIN;
	const static float MOVEMENT_GAIN;

};

