#pragma once

#include "pch.h"
#include "SceneLights.h"
#include "SubModules/Render/Core/IWindowSizeDependent.h"


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

class MyPostProcess;
class Camera;

class SceneStateObject final
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

	// �̰��� �ƴѵ�.. �����н� ������ ���̵� �ʿ��ϴ�, Move To Scene class
	void RenderProcess(ID3D11DeviceContext1* pContext, ID3D11Texture2D* pBufferToProcess, ID3D11RenderTargetView* pRTVToPresent);

	// UI, Control ������.. �ӽ÷�
	inline const std::unique_ptr<Camera>& GetCamera() const
	{
		return m_camera;
	}
	inline const std::unique_ptr<SceneLights>& GetSceneLights() const
	{
		return m_sceneLights;
	}

	inline const std::unique_ptr<MyPostProcess>& GetPostProcess()
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
	std::unique_ptr<MyPostProcess> m_postProcess; // Scene���� ��ó���� �ٸ������ݾ�? ������ ���·� ���ƾ��ϴ°�?


	std::unique_ptr<SceneLights> m_sceneLights; // TODO : move to Scene class
	std::unique_ptr<Camera> m_camera; // TODO : shared_ptr with Scene class


public:
	static float NEAR_Z;
	static float FAR_Z;
	static float FOV;
	static float SHADOW_MAP_SIZE;

};

