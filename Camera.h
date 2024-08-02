#pragma once
#include "pch.h"
#include "IDepthRenderable.h"
#include "IWindowSizeDependent.h"

class Camera final : public IDepthRenderable, IWindowSizeDependent
{
public:
	Camera(DirectX::SimpleMath::Vector3 eyePos, DirectX::SimpleMath::Vector3 viewDir, DirectX::SimpleMath::Vector3 upVector, float nearZ, float farZ, float fov);
	DirectX::SimpleMath::Matrix GetViewRow() const override;
	DirectX::SimpleMath::Vector3 GetEyePos() const;
	DirectX::SimpleMath::Vector3 GetEyeDir() const;

	void UpdatePitchYaw(DirectX::SimpleMath::Vector3& deltaRadian);
	void UpdateLookAt(DirectX::SimpleMath::Vector3& viewDir);
	void UpdatePos(DirectX::SimpleMath::Vector3& deltaPos);
	void Reset();
	DirectX::SimpleMath::Quaternion GetPitchYawInQuarternion() const;

	void OnWindowSizeChange(ID3D11Device1* pDevice, D3D11_VIEWPORT vp, DXGI_FORMAT bufferFormat) override;

	inline ID3D11ShaderResourceView* GetDepthMapSRV() const
	{
		return IDepthRenderable::m_depthTex->GetSRV();
	}



private:
	DirectX::SimpleMath::Vector3 m_eyePosWorld;
	DirectX::SimpleMath::Vector3 m_lookAtTargetPosWorld;
	DirectX::SimpleMath::Vector3 m_upVector;

	float m_pitch;
	float m_yaw;

	float m_nearZ;
	float m_farZ;
	float m_fov;

public:
	const static DirectX::XMVECTORF32 START_POSITION;
	const static float ROTATION_GAIN;
	const static float MOVEMENT_GAIN;

};

