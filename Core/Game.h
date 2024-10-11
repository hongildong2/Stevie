//
// Game.h
//
#pragma once
#include "pch.h"

#include "SubModules\Render\Core\DeviceResources.h"
#include "SubModules\Render\Scene\Camera.h"
#include "SubModules\Render\Core\GraphicsPSO.h"
#include "SubModules\Render\Core\ComputePSO.h"
#include "SubModules\Render\Scene\SceneLights.h"
#include "SubModules\Render\Scene\SceneStateObject.h"
#include "SubModules\Render\Core\MyPostProcess.h"

#include "StepTimer.h"
#include "AObject.h"
#include "SubModules/GUI/GUI.h"
#include "AObjectHandler.h"
#include "Ocean/Ocean.h"
#include "Cloud.h"



constexpr DXGI_FORMAT HDR_BUFFER_FORMAT = DXGI_FORMAT_R16G16B16A16_FLOAT;

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify, public AObjectHandler
{
	friend class SceneStateObject;
public:

	Game() noexcept(false);
	~Game() = default;

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

	// Object Management
	void Register(AObject* obj) override;
	void UnRegister(AObject* obj) override;


private:
	void Update(DX::StepTimer const& timer);

	void Render();

	void Clear();

	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();

	// Device resources.
	std::unique_ptr<DX::DeviceResources>    m_deviceResources;

	// Rendering loop timer.
	DX::StepTimer                           m_timer;

	// GUI Component
	std::unique_ptr<GUI> m_GUI;

	// Input Component
	std::unique_ptr<DirectX::Keyboard> m_keyboard;
	std::unique_ptr<DirectX::Mouse> m_mouse;
	// Use this with  Mouse::ButtonStateTracker::PRESSED for debugging. Tracking pressed button!
	DirectX::Keyboard::KeyboardStateTracker m_keys;
	DirectX::Mouse::ButtonStateTracker m_mouseButtons;

	// Gameplay, Render Components
	std::vector<std::unique_ptr<Model>> m_models;
	std::unique_ptr<Model> m_skyBox;
	std::unique_ptr<Ocean> m_ocean;
	std::unique_ptr<Cloud> m_cloud;
	const std::unique_ptr<SceneStateObject> m_sceneState; // does not share with other scene

	// Renderer Component, Different Thread
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_floatBuffer;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_floatRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_floatSRV;
};
