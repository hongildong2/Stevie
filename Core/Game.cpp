//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include "GeometryGenerator.h"
#include "SubModules/Render/D3D11/D3D11Renderer.h"
#include "Components/MeshComponent.h"
#include "Core/Camera.h"
#include "SSceneObject.h"
#include "Skybox.h"
#include "Core/Light.h"
#include "Core/Ocean.h"
#include "Core/Cloud.h"

extern void ExitGame() noexcept;

Game::Game() noexcept(false)
{
	m_pRenderer = std::make_unique<D3D11Renderer>();
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
	m_objects.reserve(1000);
	float aspectRatio = (float)width / (float)height;

	m_pRenderer->SetWindow(window, width, height);
	m_pRenderer->Initialize(TRUE, TRUE, L"./SubModules/Render/D3D11/Shaders/");


	// DEMO OBJECT
	//{
	//	MeshData sphere = geometryGenerator::MakeSphere(1.f, 20, 20);
	//	RMeshGeometry* sphereMesh = m_renderer->CreateMeshGeometry(sphere.verticies.data(), sizeof(Vertex), sphere.verticies.size(), sphere.indicies.data(), sizeof(UINT), sphere.indicies.size(), EPrimitiveTopologyType::TRIANGLE_LIST, EMeshType::BASIC);

	//	const RTexture* albedoTex = m_renderer->CreateTexture2DFromWICFile(L"./Assets/Textures/worn_shiny/worn-shiny-metal-albedo.png");
	//	const RTexture* metallicTex = m_renderer->CreateTexture2DFromWICFile(L"./Assets/Textures/worn_shiny/worn-shiny-metal-Metallic.png");
	//	const RTexture* heightTex = m_renderer->CreateTexture2DFromWICFile(L"./Assets/Textures/worn_shiny/worn-shiny-metal-Height.png");
	//	const RTexture* aoTex = m_renderer->CreateTexture2DFromWICFile(L"./Assets/Textures/worn_shiny/worn-shiny-metal-ao.png");
	//	const RTexture* normalTex = m_renderer->CreateTexture2DFromWICFile(L"./Assets/Textures/worn_shiny/worn-shiny-metal-Normal-dx.png");
	//	const RTexture* roughnessTex = m_renderer->CreateTexture2DFromWICFile(L"./Assets/Textures/worn_shiny/worn-shiny-metal-Roughness.png");

	//	RBasicMaterial* mat = new RBasicMaterial(m_renderer.get());
	//	mat->SetAlbedoTexture(albedoTex);
	//	mat->SetMetallicTexture(metallicTex);
	//	// mat->SetHeightTexture(heightTex);
	//	mat->SetAOTexture(aoTex);
	//	mat->SetNormalTexture(normalTex);
	//	mat->SetRoughnessTexture(roughnessTex);

	//	mat->Initialize();

	//	MeshComponent* demoC = new MeshComponent();

	//	demoC->Initialize(m_renderer.get());
	//	demoC->SetMeshGeometry(sphereMesh);
	//	demoC->SetMaterial(mat);
	//	for (int i = -100; i < 100; ++i)
	//	{
	//		auto demoObj = std::make_unique<SSceneObject>();


	//		demoObj->Initialize();
	//		demoObj->SetMeshComponent(demoC);

	//		auto a = Vector3(i / 2.f, i / 2.f, i / 2.f);
	//		demoObj->UpdatePos(a);
	//		m_objects.push_back(std::move(demoObj));
	//	}
	//}

	// OCEAN
	{
		auto oceanObj = std::make_unique<Ocean>(m_pRenderer.get());
		oceanObj->Initialize();
		m_objects.push_back(std::move(oceanObj));
	}

	// CLOUD
	{
		auto cloudObj = std::make_unique<Cloud>(m_pRenderer.get());
		cloudObj->Initialize();
		m_objects.push_back(std::move(cloudObj));
	}

	// Scene
	{
		//IBL 

		// Camera
		m_camera = std::make_unique<Camera>(Vector3{ 0.f, 1.f, 0.f });
		m_pRenderer->SetCamera(m_camera.get());

		// Skybox
		MeshData box = geometryGenerator::MakeBox(50.f);
		RMeshGeometry* cubeMesh = m_pRenderer->CreateMeshGeometry(box.verticies.data(), sizeof(RVertex), static_cast<UINT>(box.verticies.size()), box.indicies.data(), sizeof(UINT), static_cast<UINT>(box.indicies.size()), EPrimitiveTopologyType::TRIANGLE_LIST, EMeshType::BASIC);
		RMaterial* skyboxMaterial = new RSkyboxMaterial(m_pRenderer.get());

		const RTexture* IrradianceMapTexture = m_pRenderer->CreateTextureCubeFromDDSFile(L"./Assets/IBL/PURE_SKY/SKYEnvHDR.dds");
		const RTexture* SpecularMapTexture = m_pRenderer->CreateTextureCubeFromDDSFile(L"./Assets/IBL/PURE_SKY/SKYSpecularHDR.dds");
		const RTexture* BRDFMapTexture = m_pRenderer->CreateTexture2DFromDDSFile(L"./Assets/IBL/PURE_SKY/SKYBrdf.dds");

		m_pRenderer->SetIBLTextures(IrradianceMapTexture, SpecularMapTexture, BRDFMapTexture);

		skyboxMaterial->AddTexture(IrradianceMapTexture);
		skyboxMaterial->Initialize();


		MeshComponent* mcop = new MeshComponent();
		mcop->Initialize(m_pRenderer.get());
		mcop->SetMeshGeometry(cubeMesh);
		mcop->SetMaterial(skyboxMaterial);
		m_skybox = std::make_unique<Skybox>(mcop);

		m_pRenderer->SetSkybox(m_skybox.get());

		// Lights

		Light* light = new Light(ELightType::DIRECTIONAL);
		light->SetShadowing(TRUE);
		m_pRenderer->SetSunLight(light);
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
			m_camera->UpdateYawPitchRoll(deltaRotationRadian);
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


	for (auto& obj : m_objects)
	{
		obj->Update();
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

	m_pRenderer->BeginRender();

	for (auto& obj : m_objects)
	{
		obj->Render();
	}

	m_pRenderer->Render();

	m_pRenderer->EndRender();

	m_pRenderer->Present();
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
	renderConfig::SCREEN_WIDTH = width;
	renderConfig::SCREEN_HEIGHT = height;
	m_pRenderer->UpdateWindowSize(width, height);
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
	// TODO: Change to desired default window size (note minimum size is 320x200).
	width = 1920;
	height = 1080;
}
#pragma endregion