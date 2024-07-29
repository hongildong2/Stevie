#pragma once

#include "pch.h"
#include "GlobalLight.h"

struct GlobalConstants
{
	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix proj;
	DirectX::SimpleMath::Matrix viewProj;

	DirectX::SimpleMath::Matrix invView;
	DirectX::SimpleMath::Matrix invProj;
	DirectX::SimpleMath::Matrix invViewProj;

	DirectX::SimpleMath::Vector3 eyeWorld;
	float timeGlobal;

	UINT globalLightsCount;
	float nearZ;
	float farZ;
	float gcDummy;

	LightData globalSunLight;
};

class Game;

class SceneStateObject final
{
public:
	SceneStateObject() = default;
	~SceneStateObject() = default;

	SceneStateObject(const SceneStateObject& other) = delete;
	SceneStateObject(const SceneStateObject&& other) = delete;

	SceneStateObject& operator=(const SceneStateObject& other) = delete;
	SceneStateObject& operator=(const SceneStateObject&& other) = delete;

	void Initialize(ID3D11Device1* pDevice);
	// Game -> Scene
	void Update(ID3D11DeviceContext1* pContext, Game* pGame);
	void PrepareRender(ID3D11DeviceContext1* pContext);



private:
	GlobalConstants m_globalConstant;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_globalCB;
	// TODO : 카메라 같은 다른 오브젝트들도??
};

