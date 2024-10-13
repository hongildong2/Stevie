//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include "GeometryGenerator.h"
#include "SubModules/Render/GraphicsCommon1.h"
#include "SubModules/Render/D3D11/D3D11Renderer.h"
#include "Components/MeshComponent.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Quaternion;

Game::Game() noexcept(false)
{
	m_renderer = std::make_unique<D3D11Renderer>();
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
	m_renderer->SetWindow(window, width, height);
	m_renderer->Initialize(TRUE, TRUE, L"./SubModules/Render/D3D11/Shaders/");

	m_obj = std::make_unique<SGameObject>();

	MeshData sphere = MakeSphere(2.f, 50, 50);
	RMeshGeometry* sphereMesh = m_renderer->CreateMeshGeometry(sphere.verticies.data(), sizeof(Vertex), sphere.verticies.size(), sphere.indicies.data(), sizeof(UINT), sphere.indicies.size());
	MeshComponent* demoC = new MeshComponent();
	demoC->Initialize(sphereMesh, Graphics::DEMO_MATERIAL);

	m_obj->Initialize(m_renderer.get(), demoC);




	// TODO: Change the timer settings if you want something other than the default variable timestep mode.
	// e.g. for 60 FPS fixed timestep update logic, call:
	m_timer.SetFixedTimeStep(false);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);

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
			// Vector3 deltaRotationRadian = Vector3(float(mouse.x), float(mouse.y), 0.f) * Camera::ROTATION_GAIN;
			// m_sceneState->GetCamera()->UpdatePitchYaw(deltaRotationRadian);
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
			// m_sceneState->GetCamera()->Reset();
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
		// Quaternion q = m_sceneState->GetCamera()->GetPitchYawInQuarternion();
		// move = Vector3::Transform(move, q); // represented in camera space

		// Vector3 deltaMove = move * Camera::MOVEMENT_GAIN;
		// m_sceneState->GetCamera()->UpdatePos(deltaMove);

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

	m_obj->Render();

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


	// m_renderer->UpdateWindowSize();
}

void Game::OnDisplayChange()
{
}

void Game::OnWindowSizeChanged(int width, int height)
{
	// TODO: Game window is being resized.
	m_renderer->UpdateWindowSize(width, height);
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
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{

}

void Game::OnDeviceLost()
{
	// TODO: Add Direct3D resource cleanup here.
}

void Game::OnDeviceRestored()
{
	// TODO : reset all the resources

	CreateDeviceDependentResources();

	CreateWindowSizeDependentResources();
}
#pragma endregion
