//
// Game.h
//
#pragma once
#include "pch.h"


#include "StepTimer.h"

#include "SubModules/Render/RenderDefs.h"
#include "SubModules/Render/D3D11/D3DUtil.h"
#include "SGameObject.h"

constexpr DXGI_FORMAT HDR_BUFFER_FORMAT = DXGI_FORMAT_R16G16B16A16_FLOAT;

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
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


private:
	void Update(DX::StepTimer const& timer);

	void Render();

	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();

	// Device resources.
	std::unique_ptr<IRenderer> m_renderer;

	// Rendering loop timer.
	DX::StepTimer                           m_timer;


	// Input Component
	std::unique_ptr<DirectX::Keyboard> m_keyboard;
	std::unique_ptr<DirectX::Mouse> m_mouse;
	// Use this with  Mouse::ButtonStateTracker::PRESSED for debugging. Tracking pressed button!
	DirectX::Keyboard::KeyboardStateTracker m_keys;
	DirectX::Mouse::ButtonStateTracker m_mouseButtons;


	// Demo
	std::unique_ptr<SGameObject> m_obj;

};
