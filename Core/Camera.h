#pragma once
#include "pch.h"
#include "Core/SSceneObject.h"

class Camera final : public SSceneObject
{
public:
	Camera(DirectX::SimpleMath::Vector3 eyePos);
	~Camera() = default;

	void Reset();
	DirectX::SimpleMath::Quaternion GetPitchYawInQuarternion() const;

public:
	const static DirectX::XMVECTORF32 START_POSITION;
	const static float ROTATION_GAIN;
	const static float MOVEMENT_GAIN;

};

