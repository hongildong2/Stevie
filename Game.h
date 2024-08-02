//
// Game.h
//
#pragma once
#include "pch.h"

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "DeviceResources.h"
#include "StepTimer.h"
#include "Camera.h"
#include "GraphicsPSO.h"
#include "ComputePSO.h"


#include "Ocean.h"
#include "SceneLights.h"
#include "SceneStateObject.h"
#include "PostProcess.h"


constexpr DXGI_FORMAT HDR_BUFFER_FORMAT = DXGI_FORMAT_R16G16B16A16_FLOAT;

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
	friend class SceneStateObject;
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


	// Input Class
	std::unique_ptr<DirectX::Keyboard> m_keyboard;
	std::unique_ptr<DirectX::Mouse> m_mouse;
	// Use this with  Mouse::ButtonStateTracker::PRESSED for debugging. Tracking pressed button!
	DirectX::Keyboard::KeyboardStateTracker m_keys;
	DirectX::Mouse::ButtonStateTracker m_mouseButtons;

	/// Scene : models, lights, scene state
	// Scene members
	std::vector<std::unique_ptr<Model>> m_models;
	std::unique_ptr<Model> m_skyBox;
	std::unique_ptr<Ocean> m_ocean;
	const std::unique_ptr<SceneStateObject> m_sceneState; // does not share with other scene

	// RenderResource, RenderPass, RenderGraph, RHI
	// RenderResource, RenderPass use these RenderResource objects
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthMap;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthMapDSV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_depthMapSRV;


	// RenderResource
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_floatBuffer;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_floatRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_floatSRV;
};
