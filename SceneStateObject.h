#pragma once

#include "pch.h"
#include "SceneLights.h"
#include "IWindowSizeDependent.h"
#include "PostProcess.h"
#include "Camera.h"

struct GlobalConstants
{
	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix proj;
	DirectX::SimpleMath::Matrix viewProj;

	DirectX::SimpleMath::Matrix invView;
	DirectX::SimpleMath::Matrix invProj;
	DirectX::SimpleMath::Matrix invViewProj;

	DirectX::SimpleMath::Vector3 eyeWorld;
	float globalTime;

	DirectX::SimpleMath::Vector3 eyeDir;
	UINT globalLightsCount;

	float nearZ;
	float farZ;
	float gcDummy[2];
};

class Game;


// TOOD : GameStateObject와 RenderStateObject로 분리
class SceneStateObject final : public IWindowSizeDependent
{
public:
	SceneStateObject();
	~SceneStateObject() = default;

	SceneStateObject(const SceneStateObject& other) = delete;
	SceneStateObject(const SceneStateObject&& other) = delete;

	SceneStateObject& operator=(const SceneStateObject& other) = delete;
	SceneStateObject& operator=(const SceneStateObject&& other) = delete;

	void Initialize(ID3D11Device1* pDevice);
	void Update(ID3D11DeviceContext1* pContext);
	void PrepareRender(ID3D11DeviceContext1* pContext);

	// 이건좀 아닌듯.. 렌더패스 관리할 아이디어가 필요하다, Move To Scene class
	void RenderProcess(ID3D11DeviceContext1* pContext, ID3D11Texture2D* pBufferToProcess, ID3D11ShaderResourceView* pDepthMapSRV, ID3D11RenderTargetView* pRTVToPresent);

	void OnWindowSizeChange(ID3D11Device1* pDevice, RECT size, DXGI_FORMAT bufferFormat) override;

	// UI, Control 때문에.. 임시로
	inline const std::unique_ptr<Camera>& GetCamera() const
	{
		return m_camera;
	}
	inline const std::unique_ptr<SceneLights>& GetSceneLights() const
	{
		return m_sceneLights;
	}

	inline const std::unique_ptr<PostProcess>& GetPostProcess()
	{
		return m_postProcess;
	}


private:
	GlobalConstants m_globalConstant;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_globalCB;

	// RenderResource -> EnvironmentMap, Scene의 상태로 보아야하나
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cubemapEnvView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cubemapIrradianceView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cubemapSpecularView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cubemapBRDFView;
	std::unique_ptr<PostProcess> m_postProcess; // Scene마다 후처리가 다를수있잖아? 하지만 상태로 보아야하는가?


	std::unique_ptr<SceneLights> m_sceneLights; // TODO : move to Scene class
	std::unique_ptr<Camera> m_camera; // TODO : shared_ptr with Scene class


	DirectX::SimpleMath::Matrix m_proj;
};

