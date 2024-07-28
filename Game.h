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

#include "Ocean.h"
#include "GlobalLight.h"



constexpr float NEAR_Z = 0.1f;
constexpr float FAR_Z = 150.f;
constexpr float FOV = 90.f;

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


	// Inputs
	std::unique_ptr<DirectX::Keyboard> m_keyboard;
	std::unique_ptr<DirectX::Mouse> m_mouse;
	// Use this with  Mouse::ButtonStateTracker::PRESSED for debugging. Tracking pressed button!
	DirectX::Keyboard::KeyboardStateTracker m_keys;
	DirectX::Mouse::ButtonStateTracker m_mouseButtons;
	float m_pitch;
	float m_yaw;


	/// Scene : models, camera, viewport, lights, cubemap
	std::unique_ptr<Camera> m_camera;
	std::vector<std::unique_ptr<Model>> m_models;
	DirectX::SimpleMath::Matrix m_proj;


	// Scene - DepthOnly?
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthOnlyBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthOnlyDSV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_depthOnlySRV;
	std::unique_ptr<Ocean> m_ocean;

	// light
	GlobalLight m_globalLight;

	// Global Resources	
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cubemapEnvView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cubemapIrradianceView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cubemapSpecularView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cubemapBRDFView;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_floatBuffer;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_floatRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_floatSRV;

	std::unique_ptr<PostProcess> m_postProcess;
};
