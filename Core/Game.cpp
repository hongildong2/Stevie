//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include "GeometryGenerator.h"
#include "SubModules/Render/GraphicsCommon1.h"
#include "SubModules/Render/D3D11/D3D11Renderer.h"
#include "SubModules/Render/Scene/Camera.h"
#include "SubModules/Render/RMaterial.h"
#include "SubModules/Render/RTexture.h"
#include "Components/MeshComponent.h"
#include "SSceneObject.h"
#include "Skybox.h"

extern void ExitGame() noexcept;

using namespace DirectX;
using namespace DirectX::SimpleMath;
using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
	m_renderer = std::make_unique<D3D11Renderer>();
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
	m_objects.reserve(1000);
	float aspectRatio = (float)width / (float)height;

	m_renderer->SetWindow(window, width, height);
	m_renderer->Initialize(TRUE, TRUE, L"./SubModules/Render/D3D11/Shaders/");


	// DEMO
	for (int i = -100; i < 100; ++i)
	{
		auto demoObj = std::make_unique<SSceneObject>();
		MeshData sphere = MakeSphere(1.f, 20, 20);
		RMeshGeometry* sphereMesh = m_renderer->CreateMeshGeometry(sphere.verticies.data(), sizeof(Vertex), sphere.verticies.size(), sphere.indicies.data(), sizeof(UINT), sphere.indicies.size());
		RMaterial* mat = new RDemoMaterial(m_renderer.get());

		MeshComponent* demoC = new MeshComponent();

		demoC->Initialize(m_renderer.get(), sphereMesh, mat);

		demoObj->Initialize();
		demoObj->SetMeshComponent(demoC);

		demoObj->UpdatePos({ i / 2.f, i / 2.f, i / 2.f });
		m_objects.push_back(std::move(demoObj));
	}

	// Scene
	{
		m_camera = std::make_unique<Camera>(Vector3{ 0.f, 1.f, 0.f }, Vector3{ 0.f, 0.f, 1.f }, Vector3{ 0.f, 1.f, 0.f }, aspectRatio, 0.1f, 20.f, XM_PIDIV2);
		m_renderer->SetCamera(m_camera.get());

		MeshData box = MakeBox(10.f);
		RMeshGeometry* cubeMesh = m_renderer->CreateMeshGeometry(box.verticies.data(), sizeof(Vertex), box.verticies.size(), box.indicies.data(), sizeof(UINT), box.indicies.size());
		RMaterial* sMat = new RSkyboxMaterial(m_renderer.get());

		const RTexture* iblTex = m_renderer->CreateTextureCubeFromFile(L"./Assets/IBL/brightEnvHDR.dds");
		sMat->AddTexture(iblTex);
		sMat->Initialize();


		MeshComponent* mcop = new MeshComponent();
		mcop->Initialize(m_renderer.get(), cubeMesh, sMat);
		m_skybox = std::make_unique<Skybox>(mcop);

		m_renderer->SetSkybox(m_skybox.get());
	}

	// TODO: Change the timer settings if you want something other than the default variable timestep mode.
	// e.g. for 60 FPS fixed timestep update logic, call:
	m_timer.SetFixedTimeStep(false);
	m_timer.SetTargetElapsedSeconds(1.0f / 60.0f);

	m_keyboard = std::make_unique<Keyboard>();
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(window);
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
	// 뭔가 비동기적으로동작하는듯
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

	// Update Control, CleanUp later
	{
		auto mouse = m_mouse->GetState();
		m_mouseButtons.Update(mouse);

		if (mouse.positionMode == Mouse::MODE_RELATIVE)
		{
			Vector3 deltaRotationRadian = Vector3(float(mouse.x), float(mouse.y), 0.f) * Camera::ROTATION_GAIN;
			m_camera->UpdatePitchYaw(deltaRotationRadian);
		}

		m_mouse->SetMode(mouse.rightButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);

		auto kb = m_keyboard->GetState();
		m_keys.Update(kb);
		if (kb.Escape)
		{
			ExitGame();
		}

		if (kb.Home)
		{
			m_camera->Reset();
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


		// Get Camera PitchYaw Quarternion
		Quaternion q = m_camera->GetPitchYawInQuarternion();
		move = Vector3::Transform(move, q); // represented in camera space

		Vector3 deltaMove = move * Camera::MOVEMENT_GAIN;
		m_camera->UpdatePos(deltaMove);

		// no bound currently
		//Vector3 halfBound = (Vector3(ROOM_BOUNDS.v) / Vector3(2.f))
		//	- Vector3(0.1f, 0.1f, 0.1f);

		//m_cameraPos = Vector3::Min(m_cameraPos, halfBound);
		//m_cameraPos = Vector3::Max(m_cameraPos, -halfBound);
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

	m_renderer->BeginRender();

	for (auto& obj : m_objects)
	{
		obj->Render();
	}

	m_renderer->EndRender();

	m_renderer->Present();
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
}

void Game::OnDisplayChange()
{
}

void Game::OnWindowSizeChanged(int width, int height)
{
	// TODO: Game window is being resized.
	m_renderer->UpdateWindowSize(width, height);
	if (m_camera)
	{
		m_camera->UpdateAspectRatio((float)width / (float)height);
	}
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
	// TODO: Change to desired default window size (note minimum size is 320x200).
	width = 1920;
	height = 1080;
}
#pragma endregion