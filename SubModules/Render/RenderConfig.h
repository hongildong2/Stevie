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
	static UINT CAMERA_DEPTH_MAP_WIDTH = 2048;
	static UINT CAMERA_DEPTH_MAP_HEIGHT = 2048;

	static UINT LIGHT_DEPTH_MAP_WIDTH = 1024;
	static UINT LIGHT_DEPTH_MAP_HEIGHT = 1024;
	static FLOAT LIGHT_DEPTH_MAP_NEAR_Z = 1.f;
	static FLOAT LIGHT_DEPTH_MAP_FAR_Z = 50.f;

	static inline DirectX::SimpleMath::Matrix GetDirectionalLightProjRowMat()
	{
		return DirectX::SimpleMath::Matrix::CreateOrthographic(LIGHT_DEPTH_MAP_WIDTH, LIGHT_DEPTH_MAP_HEIGHT, LIGHT_DEPTH_MAP_NEAR_Z, LIGHT_DEPTH_MAP_FAR_Z);
	}
	static inline DirectX::SimpleMath::Matrix GetSpotLightProjRowMat()
	{
		return DirectX::SimpleMath::Matrix::CreatePerspective(LIGHT_DEPTH_MAP_WIDTH, LIGHT_DEPTH_MAP_HEIGHT, LIGHT_DEPTH_MAP_NEAR_Z, LIGHT_DEPTH_MAP_FAR_Z);
	}




}