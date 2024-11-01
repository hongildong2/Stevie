#pragma once
#include "pch.h"
#include "Core/SSceneObject.h"

class Camera final : public SSceneObject
{
public:
	Camera(DirectX::SimpleMath::Vector3 eyePos, float aspectRatio, float nearZ, float farZ, float fov);
	~Camera() = default;

	DirectX::SimpleMath::Matrix GetProjRowMat() const;

	inline float GetNearZ() const
	{
		return m_nearZ;
	}
	inline float GetFarZ() const
	{
		return m_farZ;
	}

	void UpdateAspectRatio(float aspectRatio);
	void Reset();
	DirectX::SimpleMath::Quaternion GetPitchYawInQuarternion() const;


private:
	float m_aspectRatio;
	float m_nearZ;
	float m_farZ;
	float m_fov;

public:
	const static DirectX::XMVECTORF32 START_POSITION;
	const static float ROTATION_GAIN;
	const static float MOVEMENT_GAIN;

};

