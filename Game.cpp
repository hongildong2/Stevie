//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include "Model.h"
#include "GraphicsCommon.h"
#include "Utility.h"
#include "DepthOnlyResources.h"
#include "AObject.h"
#include "AObjectManager.h"
#include "IComponentManager.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Quaternion;

Game::Game() noexcept(false) :
	m_sceneState(std::make_unique<SceneStateObject>())
{
	m_deviceResources = std::make_unique<DX::DeviceResources>();
	m_deviceResources->RegisterDeviceNotify(this);

	/* TODO::Load Asset, Model, Environment Here Before Initialize Call.
	* If AObject is constructed, AObjectManager will automatically Register Objects Here To Underlying Components
	* Then Initialize Call will Initialize It All.
	*
	*/

	auto* man = AObjectManager::GetInstance();
	auto comp = IComponentManager::GetInstance();


	man->RegisterIObjectHandler(this);

	m_GUI = std::make_unique<GUI>();
	comp->RegisterComponentHandler(m_GUI.get());
}

// Resource Management
void Game::Register(AObject* obj)
{
	assert(obj != nullptr);
}

// Resource Management
void Game::UnRegister(AObject* obj)
{
	assert(obj != nullptr);
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
	m_deviceResources->SetWindow(window, width, height);
	m_deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();


	m_deviceResources->CreateWindowSizeDependentResources();
	CreateWindowSizeDependentResources();

	// GUI Init
	{
		auto* device = m_deviceResources->GetD3DDevice();
		auto* context = m_deviceResources->GetD3DDeviceContext();
		if (m_GUI->Initialize(device, context, window, width, height) == false)
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
	// 뭔가 비동기적으로동작하는듯
	m_timer.Tick([&]()
		{
			Update(m_timer);
		});

	// update envirioment
	m_deviceResources->PIXBeginEvent(L"OceanUpdate");
	auto* context = m_deviceResources->GetD3DDeviceContext();
	m_ocean->Update(context);
	m_deviceResources->PIXEndEvent();


	for (auto& modelPtr : m_models)
	{
		modelPtr->Update(context);
	}
	m_sceneState->Update(context);


	Render();
}


// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	float elapsedTime = float(timer.GetElapsedSeconds());

	m_GUI->Update();

	// Update Control, CleanUp later
	{
		auto mouse = m_mouse->GetState();
		m_mouseButtons.Update(mouse);

		if (mouse.positionMode == Mouse::MODE_RELATIVE)
		{
			Vector3 deltaRotationRadian = Vector3(float(mouse.x), float(mouse.y), 0.f)
				* Camera::ROTATION_GAIN;
			m_sceneState->GetCamera()->UpdatePitchYaw(deltaRotationRadian);
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
			m_sceneState->GetCamera()->Reset();
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
		Quaternion q = m_sceneState->GetCamera()->GetPitchYawInQuarternion();
		move = Vector3::Transform(move, q); // represented in camera space

		Vector3 deltaMove = move * Camera::MOVEMENT_GAIN;
		m_sceneState->GetCamera()->UpdatePos(deltaMove);

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

	// clear renderTarget, clear depth-stencil buffer => set renderTarget with depth-stencil buffer, set viewport
	Clear();

	auto context = m_deviceResources->GetD3DDeviceContext();

	m_deviceResources->PIXBeginEvent(L"Scene");
	// Scene.Draw()
	{

		m_deviceResources->PIXBeginEvent(L"DepthOnlyPass");
		// DepthOnlyPass
		{
			auto* dor = DepthOnlyResources::GetInstance();
			dor->BeginDepthOnlyPass(context);
			const auto& createDEEPPTHHH = dor->GetDepthRenderableObjects();
			for (auto& obj : createDEEPPTHHH)
			{
				obj->SetContextDepthOnly(context);

				for (auto& model : m_models)
				{
					model->RenderOverride(context, Graphics::depthOnlyPSO);
				}
				m_skyBox->RenderOverride(context, Graphics::cubeMapDepthOnlyPSO);
				m_ocean->RenderOverride(context, Graphics::Ocean::depthOnlyPSO);

			}
			dor->EndDepthOnlyPass(context);
		}
		m_deviceResources->PIXEndEvent();


		// 글로벌 상태, 공용 리소스 설정
		m_sceneState->PrepareRender(context);
		// Models
		{
			m_deviceResources->PIXBeginEvent(L"Models");
			for (auto& model : m_models)
			{
				model->Render(context);
			}
			m_deviceResources->PIXEndEvent();
		}

		// Ocean
		{
			m_deviceResources->PIXBeginEvent(L"OceanPlane");
			m_ocean->Render(context);
			m_deviceResources->PIXEndEvent();
		}
		m_skyBox->Render(context);

	}
	m_deviceResources->PIXEndEvent();

	m_deviceResources->PIXBeginEvent(L"PostProcess");
	{
		auto* rtv = m_deviceResources->GetRenderTargetView();
		m_sceneState->RenderProcess(context, m_floatBuffer.Get(), rtv);
	}
	m_deviceResources->PIXEndEvent();


	m_GUI->Render();

	// Show the new frame.
	m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
	m_deviceResources->PIXBeginEvent(L"Clear");
	// 여길 정리할 방법은??
	// Clear the views.
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto depthStencil = m_deviceResources->GetDepthStencilView();
	auto* backBufferRTV = m_deviceResources->GetRenderTargetView();

	context->ClearRenderTargetView(m_floatRTV.Get(), Colors::Black); // HDR Pipeline, using float RTV
	context->ClearRenderTargetView(backBufferRTV, Colors::Black);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ID3D11RenderTargetView* rtvs[1] = {
		m_floatRTV.Get(),
	};
	context->OMSetRenderTargets(1, rtvs, depthStencil);

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
	auto* device = m_deviceResources->GetD3DDevice();

	Graphics::InitCommonStates(device);
	auto* dop = DepthOnlyResources::GetInstance();

	dop->InitDepthOnlyResources(device);

	// MAKE SCENE CLASS PLEASE
	{
		// Sample model
		{
			MeshData sphereMesh = GeometryGenerator::MakeSphere(0.5f, 100, 100);

			TextureFiles texes =
			{
				L"./Assets/Textures/worn_shiny/worn-shiny-metal-albedo.png",
					L"./Assets/Textures/worn_shiny/worn-shiny-metal-ao.png",
					L"./Assets/Textures/worn_shiny/worn-shiny-metal-Height.png",
					L"./Assets/Textures/worn_shiny/worn-shiny-metal-Metallic.png",
					L"./Assets/Textures/worn_shiny/worn-shiny-metal-Normal-dx.png",
					L"./Assets/Textures/worn_shiny/worn-shiny-metal-Roughness.png"
			};
			std::unique_ptr<MeshPart> sph = std::make_unique<MeshPart>(sphereMesh, EMeshType::SOLID, device, texes);
			std::unique_ptr<Model> smaple = std::make_unique<Model>("Sample Sphere", EModelType::DEFAULT, Graphics::basicPSO);
			smaple->AddMeshComponent(std::move(sph));
			smaple->Initialize(device);
			smaple->UpdatePosByTransform(DirectX::SimpleMath::Matrix::CreateTranslation(0.f, 5.f, 0.f));

			m_models.push_back(std::move(smaple));


			MeshData plane = GeometryGenerator::MakeSquare(5.f);
			std::unique_ptr<MeshPart> plane2 = std::make_unique<MeshPart>(plane, EMeshType::SOLID, device, texes);
			std::unique_ptr<Model> samplane = std::make_unique<Model>("Sample Plane", EModelType::DEFAULT, Graphics::basicPSO);
			samplane->AddMeshComponent(std::move(plane2));
			samplane->Initialize(device);
			samplane->UpdatePosByTransform(DirectX::SimpleMath::Matrix::CreateRotationX(DirectX::XM_PIDIV2) * DirectX::SimpleMath::Matrix::CreateTranslation(0.f, 1.5f, 0.f));

			m_models.push_back(std::move(samplane));
		}

		// Ocean
		{
			m_ocean = std::make_unique<Ocean>();
			MeshData quadPatches;
			GeometryGenerator::MakeCWQuadPatches(128, &quadPatches);
			auto tessellatedQuads = std::make_unique<MeshPart>(quadPatches, EMeshType::TESSELLATED, device, NO_MESH_TEXTURE);

			// material
			Material mat = DEFAULT_MATERIAL;
			mat.bUseTexture = FALSE;
			mat.specular = 0.255f; // unreal's water specular
			mat.albedo = { 0.f, 41.f / 255.f, 73.f / 255.f };

			tessellatedQuads->UpdateMaterialConstant(mat);
			m_ocean->AddMeshComponent(std::move(tessellatedQuads));

			// size
			auto manipulate = Matrix::CreateScale(ocean::WORLD_SCALER);
			manipulate *= Matrix::CreateRotationX(DirectX::XM_PIDIV2);
			m_ocean->UpdatePosByTransform(manipulate);

			m_ocean->Initialize(device);
		}

		// Cubemap
		{
			MeshData cube = GeometryGenerator::MakeBox(75.f);
			auto cubeMesh = std::make_unique<MeshPart>(cube, EMeshType::SOLID, device, NO_MESH_TEXTURE);
			m_skyBox = std::make_unique<Model>("cubeMap", EModelType::DEFAULT, Graphics::cubemapPSO);
			m_skyBox->AddMeshComponent(std::move(cubeMesh));
			m_skyBox->Initialize(device);
		}
		m_sceneState->Initialize(device);
	}
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
	D3D11_VIEWPORT screenVP = m_deviceResources->GetScreenViewport();
	auto* pDevice = m_deviceResources->GetD3DDevice();

	m_sceneState->OnWindowSizeChange(pDevice, screenVP, HDR_BUFFER_FORMAT);


	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Format = HDR_BUFFER_FORMAT; // for HDR Pipeline
	desc.Width = screenVP.Width;
	desc.Height = screenVP.Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = 0;

	// HDR Pipeline
	DX::ThrowIfFailed(pDevice->CreateTexture2D(&desc, NULL, m_floatBuffer.GetAddressOf()));
	CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R16G16B16A16_FLOAT);
	DX::ThrowIfFailed(pDevice->CreateRenderTargetView(m_floatBuffer.Get(), &renderTargetViewDesc, m_floatRTV.ReleaseAndGetAddressOf()));
	DX::ThrowIfFailed(pDevice->CreateShaderResourceView(m_floatBuffer.Get(), NULL, m_floatSRV.GetAddressOf()));

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
