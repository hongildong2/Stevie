//
// Game.cpp
//
#include <string>
#include <iostream>
#include <vector>

#include "pch.h"
#include "Game.h"

#include "Model.h"
#include "GraphicsCommon.h"
#include "Utility.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Matrix;


Game::Game() noexcept(false)
{
	m_deviceResources = std::make_unique<DX::DeviceResources>();
	// TODO: Provide parameters for swapchain format, depth/stencil format, and backbuffer count.
	//   Add DX::DeviceResources::c_AllowTearing to opt-in to variable rate displays.
	//   Add DX::DeviceResources::c_EnableHDR for HDR10 display.
	m_camera = Camera(DirectX::SimpleMath::Vector3(0.f, 0.2f, -5.f), Vector3(0.f, 0.f, 1.f), DirectX::SimpleMath::Vector3::UnitY);

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



	// TODO: Change the timer settings if you want something other than the default variable timestep mode.
	// e.g. for 60 FPS fixed timestep update logic, call:
	m_timer.SetFixedTimeStep(true);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);
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

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	float elapsedTime = float(timer.GetElapsedSeconds());

	// TODO: Add your game logic here.

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
	RECT screenSize = m_deviceResources->GetOutputSize();
	auto projectionMatrix = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(XM_PIDIV4,
		float(screenSize.right) / float(screenSize.bottom), 0.01f, 100.f);
	auto viewMatrix = m_camera.GetViewMatrix();


	// Scene.Draw()
	{
		// Light.Draw()
		Utility::DXResource::UpdateConstantBuffer(m_lightsConstantsCPU, context, m_lightsConstantBuffer);
		context->PSSetConstantBuffers(1, 1, m_lightsConstantBuffer.GetAddressOf());

		const Vector3 eyePos = m_camera.GetEyePos(); // 지금은 이것만

		// 큐브맵 먼저 렌더
		m_cubeMap->PrepareForRendering(context, Graphics::basicRS, Graphics::basicIL, Graphics::cubemapVS, Graphics::cubemapPS, viewMatrix, projectionMatrix, eyePos);
		context->PSSetSamplers(0, 1, Graphics::linearWrapSS.GetAddressOf());
		m_cubeMap->Draw(context);


		for (Model& model : m_models)
		{
			model.PrepareForRendering(context, Graphics::basicRS, Graphics::basicIL, Graphics::basicVS, Graphics::basicPS, viewMatrix, projectionMatrix, eyePos);
			model.Draw(context);
		}
	}



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
		std::reverse(cube.indicies.begin(), cube.indicies.end()); // 박스 안쪽에서 렌더 하기 위해
		ModelMeshPart cubeMesh = ModelMeshPart(cube, device);
		std::vector<ModelMeshPart> meshes1 = { cubeMesh };
		m_cubeMap = std::unique_ptr<Model>(new Model("cubeMap", meshes1, Vector3(0.f, 0.f, 0.f)));
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
