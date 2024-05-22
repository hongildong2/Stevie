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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

	// Update Constant Buffer, CPU -> GPU
	{
		// 지금은 필요없음
		// Utility::DXResource::UpdateConstantBuffer(m_lightsConstantsCPU, context, m_lightsConstantBuffer);
		m_PSConstantsCPU.eyeWorld = m_camera.GetEyePos(); // 지금은 이것만
		Utility::DXResource::UpdateConstantBuffer(m_PSConstantsCPU, context, m_PSConstantBuffer);
	}

	// Scene.Draw()
	{

		for (Model& model : m_models)
		{

			model.PrepareForRendering(context, Graphics::basicRS, Graphics::basicIL, Graphics::basicVS, Graphics::basicPS, viewMatrix, projectionMatrix);

			ID3D11Buffer* buffers[] = { m_lightsConstantBuffer.Get(), m_PSConstantBuffer.Get() };
			context->PSSetConstantBuffers(1, 2, buffers);
			context->PSSetSamplers(0, 1, Graphics::linearWrapSS.GetAddressOf());
			context->PSSetShaderResources(0, 1, m_textureView.GetAddressOf());
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

	// Init Assets
	{
		MeshData sphere = GeometryGenerator::MakeSphere(1.f, 100, 100);
		ModelMeshPart mesh = ModelMeshPart(sphere, device);
		std::vector<ModelMeshPart> meshes = { mesh };
		m_models.push_back(Model("BASIC SPHERE", meshes, DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f), device));
	}

	m_PSConstantsCPU.useTexture = true;
	m_PSConstantsCPU.eyeWorld = m_camera.GetEyePos();

	Utility::DXResource::CreateConstantBuffer(m_lightsConstantsCPU, device, m_lightsConstantBuffer);
	Utility::DXResource::CreateConstantBuffer(m_PSConstantsCPU, device, m_PSConstantBuffer);

	// Texture
	{
		DX::ThrowIfFailed(
			CreateWICTextureFromFile(device, L"earth.bmp", m_texture.GetAddressOf(),
				m_textureView.ReleaseAndGetAddressOf()));

		// CreateDDSTextureFromFile;
	}
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
