#pragma once
#include "pch.h"

namespace renderConfig
{
	// Screen
	static UINT SCREEN_WIDTH = 1920;
	static UINT SCREEN_HEIGHT = 1080;
	static inline FLOAT GetScreenAspectRatio()
	{
		return (FLOAT)SCREEN_WIDTH / SCREEN_HEIGHT;
	}

	// Camera
	static FLOAT CAMERA_NEAR_Z = 0.1f;
	static FLOAT CAMERA_FAR_Z = 20.f;
	static FLOAT CAMERA_FOV = DirectX::XM_PIDIV2;

	static inline DirectX::SimpleMath::Matrix GetCameraProjRowMat()
	{
		return DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(CAMERA_FOV, GetScreenAspectRatio(), CAMERA_NEAR_Z, CAMERA_FAR_Z);
	}

	// DepthMap



}