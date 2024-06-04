//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "Camera.h"
#include "GraphicsPSO.h"
#include "ComputePSO.h"
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

// ¿œ¥‹..
struct Light
{
	DirectX::SimpleMath::Vector3 strength = DirectX::SimpleMath::Vector3(1.0f);              // 12
	float fallOffStart = 0.0f;                     // 4
	DirectX::SimpleMath::Vector3 direction = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f); // 12
	float fallOffEnd = 10.0f;                      // 4
	DirectX::SimpleMath::Vector3 position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -2.0f); // 12
	float spotPower = 100.0f;                        // 4
};

struct LightConstants
{
	Light dirLight;
	Light pointLight;
	Light spotLight;
};



// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:

	Game() noexcept(false);
	~Game() = default; // TODO : destroy cubemap

	Game(Game&&) = default;
	Game& operator= (Game&&) = default;

	Game(Game const&) = delete;
	Game& operator= (Game const&) = delete;

	// Initialization and management
	void Initialize(HWND window, int width, int height);

	// Basic game loop
	void Tick();

	// IDeviceNotify
	void OnDeviceLost() override;
	void OnDeviceRestored() override;

	// Messages
	void OnActivated();
	void OnDeactivated();
	void OnSuspending();
	void OnResuming();
	void OnWindowMoved();
	void OnDisplayChange();
	void OnWindowSizeChanged(int width, int height);

	// Properties
	void GetDefaultSize(int& width, int& height) const noexcept;

private:


	void Update(DX::StepTimer const& timer);
	void UpdateGUI();

	void Render();

	void Clear();

	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();

	// Device resources.
	std::unique_ptr<DX::DeviceResources>    m_deviceResources;

	// Rendering loop timer.
	DX::StepTimer                           m_timer;

	// control
	std::unique_ptr<DirectX::Keyboard> m_keyboard;
	std::unique_ptr<DirectX::Mouse> m_mouse;
	// Use this with  Mouse::ButtonStateTracker::PRESSED for debugging. Tracking pressed button!
	DirectX::Keyboard::KeyboardStateTracker m_keys;
	DirectX::Mouse::ButtonStateTracker m_mouseButtons;

	float m_pitch;
	float m_yaw;

	// TODO : Add Input Controller

	/// Scene : models, camera, viewport, lights, cubemap
	std::vector<Model> m_models;
	std::unique_ptr<Camera> m_camera;
	DirectX::SimpleMath::Matrix m_proj;

	// light
	LightConstants m_lightsConstantsCPU;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_lightsConstantBuffer;

	// cubemap
	std::unique_ptr<Model> m_cubeMap;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cubemapEnvView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cubemapIrradianceView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cubemapSpecularView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cubemapBRDFView;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_floatBuffer;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_floatRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_floatSRV;

	std::unique_ptr<PostProcess> m_postProcess;
};
