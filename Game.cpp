//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include <string>
#include <iostream>
#include <vector>
#include "Model.h"
#include "GraphicsCommon.h"
#include "Utility.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Quaternion;

Game::Game() noexcept(false) :
	m_pitch(0),
	m_yaw(0)
{
	m_deviceResources = std::make_unique<DX::DeviceResources>();
	// TODO: Provide parameters for swapchain format, depth/stencil format, and backbuffer count.
	//   Add DX::DeviceResources::c_AllowTearing to opt-in to variable rate displays.
	//   Add DX::DeviceResources::c_EnableHDR for HDR10 display.
	m_camera = std::make_unique<Camera>(DirectX::SimpleMath::Vector3(0.f, 0.2f, -5.f), Vector3(0.f, 0.f, 1.f), DirectX::SimpleMath::Vector3::UnitY);

	m_cubeMap = nullptr;
	m_deviceResources->RegisterDeviceNotify(this);
}


// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
	m_deviceResources->SetWindow(window, width, height);

	// init light
	{
		auto& dirLight = m_lightsConstantsCPU.dirLight;
		dirLight.position = Vector3(2.f, 2.f, 0.f);
		dirLight.direction = Vector3(-5.f, -5.f, 0.f);
	}

	m_deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();


	m_deviceResources->CreateWindowSizeDependentResources();
	CreateWindowSizeDependentResources();


	// GUI Init
	{
		auto* device = m_deviceResources->GetD3DDevice();
		auto* context = m_deviceResources->GetD3DDeviceContext();
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;
		io.DisplaySize = ImVec2(float(width), float(height));
		ImGui::StyleColorsLight();

		// Setup Platform/Renderer backends
		if (!ImGui_ImplDX11_Init(device, context))
		{
			ExitGame();
		}

		if (!ImGui_ImplWin32_Init(window))
		{
			ExitGame();
		}
	}


	// TODO: Change the timer settings if you want something other than the default variable timestep mode.
	// e.g. for 60 FPS fixed timestep update logic, call:
	m_timer.SetFixedTimeStep(true);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);

	m_keyboard = std::make_unique<Keyboard>();
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(window);
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
	m_timer.Tick([&]()
		{
			Update(m_timer);
		});

	Render();
}
void Game::UpdateGUI()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();
	ImGui::Begin("Scene Control");

	ImGui::Text("Average %.3f ms/frame (%.1f FPS)",
		1000.0f / ImGui::GetIO().Framerate,
		ImGui::GetIO().Framerate);

	// GUI Controllers
	{
		ImGui::SetNextItemOpen(false, ImGuiCond_Once);
	}

	ImGui::End();
	ImGui::Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	float elapsedTime = float(timer.GetElapsedSeconds());

	UpdateGUI();

	// Update Control, CleanUp later
	{
		auto mouse = m_mouse->GetState();
		m_mouseButtons.Update(mouse);

		if (mouse.positionMode == Mouse::MODE_RELATIVE)
		{
			Vector3 delta = Vector3(float(mouse.x), float(mouse.y), 0.f)
				* Camera::ROTATION_GAIN;

			m_pitch -= delta.y;
			m_yaw -= delta.x;

		}

		m_mouse->SetMode(mouse.leftButton
			? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);

		auto kb = m_keyboard->GetState();
		m_keys.Update(kb);
		if (kb.Escape)
		{
			ExitGame();
		}

		if (kb.Home)
		{
			m_camera->Reset();
			m_pitch = m_yaw = 0;
		}

		Vector3 move = Vector3::Zero;

		if (kb.PageUp || kb.Q)
			move.y += 1.f;

		if (kb.PageDown || kb.E)
			move.y -= 1.f;

		if (kb.Left || kb.A)
			move.x += 1.f;

		if (kb.Right || kb.D)
			move.x -= 1.f;

		if (kb.Up || kb.W)
			move.z += 1.f;

		if (kb.Down || kb.S)
			move.z -= 1.f;

		Quaternion q = Quaternion::CreateFromYawPitchRoll(m_yaw, m_pitch, 0.f);

		move = Vector3::Transform(move, q);

		move *= Camera::MOVEMENT_GAIN;

		m_camera->UpdatePosBy(move);

		// no bound currently
		//Vector3 halfBound = (Vector3(ROOM_BOUNDS.v) / Vector3(2.f))
		//	- Vector3(0.1f, 0.1f, 0.1f);

		//m_cameraPos = Vector3::Min(m_cameraPos, halfBound);
		//m_cameraPos = Vector3::Max(m_cameraPos, -halfBound);

		// MOUSE : limit pitch to straight up or straight down
		constexpr float limit = XM_PIDIV2 - 0.01f;
		m_pitch = std::max(-limit, m_pitch);
		m_pitch = std::min(+limit, m_pitch);

		// keep longitude in sane range by wrapping
		if (m_yaw > XM_PI)
		{
			m_yaw -= XM_2PI;
		}
		else if (m_yaw < -XM_PI)
		{
			m_yaw += XM_2PI;
		}

		float y = sinf(m_pitch);
		float r = cosf(m_pitch);
		float z = r * cosf(m_yaw);
		float x = r * sinf(m_yaw);
		m_camera->UpdateLookAtBy(Vector3(x, y, z));
	}


	// Scene.Update() : Update light and etc. CPU info
	{

	}

	elapsedTime;
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
	{
		return;
	}

	// clear renderTarget, clear depth-stencil buffer => set renderTarget with depth-stencil buffer, set viewport
	Clear();

	m_deviceResources->PIXBeginEvent(L"Render");
	auto context = m_deviceResources->GetD3DDeviceContext();

	auto viewMatrix = m_camera->GetViewMatrix();


	// Scene.Draw()
	{
		// Light.Draw()
		Utility::DXResource::UpdateConstantBuffer(m_lightsConstantsCPU, context, m_lightsConstantBuffer);
		context->PSSetConstantBuffers(1, 1, m_lightsConstantBuffer.GetAddressOf());

		const Vector3 eyePos = m_camera->GetEyePos(); // 지금은 이것만

		// 큐브맵 먼저 렌더
		m_cubeMap->PrepareForRendering(context, Graphics::basicRS, Graphics::basicIL, Graphics::cubemapVS, Graphics::cubemapPS, viewMatrix, m_proj, eyePos);
		context->PSSetSamplers(0, 1, Graphics::linearWrapSS.GetAddressOf());
		m_cubeMap->Draw(context);


		for (Model& model : m_models)
		{
			model.PrepareForRendering(context, Graphics::basicRS, Graphics::basicIL, Graphics::basicVS, Graphics::basicPS, viewMatrix, m_proj, eyePos);
			model.Draw(context);
		}
	}


	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	m_deviceResources->PIXEndEvent();

	// Show the new frame.
	m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
	m_deviceResources->PIXBeginEvent(L"Clear");

	// Clear the views.
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTarget = m_deviceResources->GetRenderTargetView();
	auto depthStencil = m_deviceResources->GetDepthStencilView();

	context->ClearRenderTargetView(renderTarget, Colors::RoyalBlue);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);

	// Set the viewport.
	auto const viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
	// TODO: Game is becoming active window.
	m_keys.Reset();
	m_mouseButtons.Reset();
}

void Game::OnDeactivated()
{
	// TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
	// TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
	m_timer.ResetElapsedTime();
	m_keys.Reset();
	m_mouseButtons.Reset();
	// TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
	auto const r = m_deviceResources->GetOutputSize();
	m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnDisplayChange()
{
	m_deviceResources->UpdateColorSpace();
}

void Game::OnWindowSizeChanged(int width, int height)
{
	if (!m_deviceResources->WindowSizeChanged(width, height))
		return;

	CreateWindowSizeDependentResources();

	// TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
	// TODO: Change to desired default window size (note minimum size is 320x200).
	width = 1920;
	height = 1080;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
	auto device = m_deviceResources->GetD3DDevice();


	Graphics::InitCommonStates(device);



	// Texture
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(device, L"earth.bmp", nullptr,
			textureView.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(CreateDDSTextureFromFileEx(device, L"skybox.dds", 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, DDS_LOADER_FLAGS(false), m_cubemapTexture.GetAddressOf(), m_cubemapTextureView.ReleaseAndGetAddressOf(), nullptr));


	// Init Assets
	{
		// cubemap Init
		MeshData cube = GeometryGenerator::MakeBox(20.f);
		std::reverse(cube.indicies.begin(), cube.indicies.end()); // 박스 안쪽에서 렌더 하기 위해 또는 래스터라이저 설정 바꿔주기
		ModelMeshPart cubeMesh = ModelMeshPart(cube, device);
		std::vector<ModelMeshPart> meshes1 = { cubeMesh };
		m_cubeMap = std::make_unique<Model>("cubeMap", meshes1, Vector3(0.f, 0.f, 0.f));
		m_cubeMap->Initialize(device, m_cubemapTextureView);


		MeshData sphereMesh = GeometryGenerator::MakeSphere(1.f, 100, 100);
		ModelMeshPart mesh = ModelMeshPart(sphereMesh, device);
		std::vector<ModelMeshPart> meshes = { mesh };
		Model&& sphereModel = Model("BASIC SPHERE", meshes, DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f));
		sphereModel.Initialize(device, textureView);
		m_models.push_back(sphereModel);
	}

	Utility::DXResource::CreateConstantBuffer(m_lightsConstantsCPU, device, m_lightsConstantBuffer);
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
	auto size = m_deviceResources->GetOutputSize();
	m_proj = Matrix::CreatePerspectiveFieldOfView(
		XMConvertToRadians(90.f),
		float(size.right) / float(size.bottom), 0.1f, 100.f);
	// TODO: Initialize windows-size dependent objects here.
}

void Game::OnDeviceLost()
{
	// TODO: Add Direct3D resource cleanup here.
}

void Game::OnDeviceRestored()
{
	CreateDeviceDependentResources();

	CreateWindowSizeDependentResources();
}
#pragma endregion
