#pragma once

#include "pch.h"
#include "GlobalLight.h"
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

	UINT globalLightsCount;
	float nearZ;
	float farZ;
	float gcDummy;
};

class Game;

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

	// �̰��� �ƴѵ�.. �����н� ������ ���̵� �ʿ��ϴ�
	void ProcessRender(ID3D11DeviceContext1* pContext, ID3D11Texture2D* pBufferToProcess, ID3D11ShaderResourceView* pDepthMapSRV, ID3D11RenderTargetView* pRTVToPresent);

	void OnWindowSizeChange(ID3D11Device1* pDevice, RECT size, DXGI_FORMAT bufferFormat) override;

	// UI, Control ������.. �ӽ÷�
	inline const std::unique_ptr<Camera>& GetCamera()
	{
		return m_camera;
	}

	inline const std::unique_ptr<PostProcess>& GetPostProcess()
	{
		return m_postProcess;
	}


private:
	GlobalConstants m_globalConstant;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_globalCB;

	// RenderResource -> EnvironmentMap, Scene�� ���·� ���ƾ��ϳ�
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cubemapEnvView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cubemapIrradianceView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cubemapSpecularView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cubemapBRDFView;
	std::unique_ptr<PostProcess> m_postProcess; // Scene���� ��ó���� �ٸ������ݾ�? ������ ���·� ���ƾ��ϴ°�?


	std::unique_ptr<SceneLights> m_sceneLights; // TODO : move to Scene class
	std::unique_ptr<Camera> m_camera; // TODO : shared_ptr with Scene class


	DirectX::SimpleMath::Matrix m_proj;
};

