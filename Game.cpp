//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include <string>
#include <iostream>
#include <vector>
#include <string>

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
	m_yaw(0),
	m_ocean()
{
	// for post processing in compute shader
	m_deviceResources = std::make_unique<DX::DeviceResources>();
	// TODO: Provide parameters for swapchain format, depth/stencil format, and backbuffer count.
	//   Add DX::DeviceResources::c_AllowTearing to opt-in to variable rate displays.
	//   Add DX::DeviceResources::c_EnableHDR for HDR10 display.


	m_camera = std::make_unique<Camera>(DirectX::SimpleMath::Vector3(0.f, 0.2f, -5.f), Vector3(0.f, 0.f, 1.f), DirectX::SimpleMath::Vector3::UnitY);

	m_postProcess = nullptr;
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
	// 뭔가 비동기적으로동작하는듯
	m_timer.Tick([&]()
		{
			Update(m_timer);
		});

	// update envirioment
	m_deviceResources->PIXBeginEvent(L"OceanUpdate");
	auto context = m_deviceResources->GetD3DDeviceContext();
	m_ocean->Update(context);
	m_deviceResources->PIXEndEvent();

	for (auto& modelPtr : m_models)
	{
		auto pos = modelPtr->GetWorldPos();
		float height = m_ocean->GetHeight({ pos.x, pos.z });

		modelPtr->UpdatePosByCoordinate({ pos.x, height - 0.2f, pos.z, 1.f }); // ㅋㅋㅋㅋ
	}

	// update game by timer

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

	// Controller, Update DTOs
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode("Light"))
		{
			// Vector3 <-> float3는 이렇게
			ImGui::SliderFloat3("Dir", &m_lightsConstantsCPU.dirLight.direction.x, -1.f, 1.f);
			ImGui::SliderFloat3("Strength", &m_lightsConstantsCPU.dirLight.strength.x, 0.f, 3.f);
			ImGui::SliderFloat3("Position", &m_lightsConstantsCPU.dirLight.position.x, -2.f, 3.f);
			ImGui::TreePop();
		}


		if (ImGui::TreeNode("ModelConstant"))
		{
			unsigned int nth = 0;
			char buf[20];
			for (auto& model : m_models)
			{
				snprintf(buf, 20, "%d%s", nth++, "th model");
				if (ImGui::TreeNode(buf))
				{
					Material mat = model->GetMaterialConstant();

					// Vector3 <-> float3는 이렇게
					ImGui::SliderFloat("metallic", &mat.metallicFactor, 0.f, 1.f);
					ImGui::SliderFloat("ao", &mat.aoFactor, 0.f, 1.f);
					ImGui::SliderFloat("roughness", &mat.roughnessFactor, 0.f, 1.f);
					ImGui::SliderFloat("t1", &mat.t1, 0.f, 10.f);


					model->UpdateMaterialConstant(mat);

					ImGui::TreePop();

				}
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("ImageFilter"))
		{
			PostProcessConstant constant = m_postProcess->GetConstant();
			ImGui::SliderFloat("strength", &constant.strength, 0.f, 1.f);
			ImGui::SliderFloat("exposure", &constant.exposure, 0.f, 3.f);
			ImGui::SliderFloat("gamma", &constant.gamma, 0.f, 3.f);

			m_postProcess->UpdateConstant(constant);
			ImGui::TreePop();
		}
	}
	// TODO : 쉐이더, 월드에 종속적인 파라미터들은 Game에 멤버로넣고 따로 constant buffer로 떼서 업데이트하기.
// Call controllers
/*
* light.updateDir(vec3)
* light.updateStrength(float);
*
*
*/
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

		m_mouse->SetMode(mouse.rightButton
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
	auto viewMatrix = m_camera->GetViewMatrix();

	m_deviceResources->PIXBeginEvent(L"Scene");
	// Scene.Draw()
	{
		// DepthOnly Pass


		// Light.Draw()
		Utility::DXResource::UpdateConstantBuffer(m_lightsConstantsCPU, context, m_lightsConstantBuffer);
		context->PSSetConstantBuffers(1, 1, m_lightsConstantBuffer.GetAddressOf());

		const Vector3 eyePos = m_camera->GetEyePos(); // 지금은 이것만

		// IBL을 위해 0~3번에 큐브맵 텍스쳐 고정
		ID3D11ShaderResourceView* resources[] = {
			m_cubemapEnvView.Get(),
			m_cubemapIrradianceView.Get(),
			m_cubemapSpecularView.Get(),
			m_cubemapBRDFView.Get(),
		};

		ID3D11SamplerState* samplers[] = {
			Graphics::linearWrapSS.Get(),
			Graphics::linearClampSS.Get()
		};
		context->PSSetSamplers(0, 2, samplers);

		// CubeMap
		{
			m_deviceResources->PIXBeginEvent(L"CubeMap");
			context->PSSetShaderResources(0, 4, m_cubemapEnvView.GetAddressOf());

			m_cubeMap->PrepareForRendering(context, viewMatrix, m_proj, eyePos);
			m_cubeMap->Draw(context);
			m_deviceResources->PIXEndEvent();
		}

		context->VSSetSamplers(0, 2, samplers);



		// Models
		{
			m_deviceResources->PIXBeginEvent(L"Models");
			for (auto& model : m_models)
			{
				model->PrepareForRendering(context, viewMatrix, m_proj, eyePos);
				model->Draw(context);
			}
			m_deviceResources->PIXEndEvent();
		}

		// Ocean
		{
			m_deviceResources->PIXBeginEvent(L"OceanPlane");

			ID3D11ShaderResourceView* SRVs[3] = { m_ocean->GetDisplacementMapsSRV(), m_ocean->GetDerivativeMapsSRV(), m_ocean->GetCombineParameterSRV() };
			context->DSSetShaderResources(0, 3, SRVs);

			ID3D11SamplerState* SSs[1] = { Graphics::linearMirrorSS.Get() };
			context->DSSetSamplers(0, 1, SSs);

			m_oceanPlane->PrepareForRendering(context, viewMatrix, m_proj, eyePos); // 애초에 리소스 같은 자잘한게 이메서드에서 다형적으로 전부 처리되어야지..
			ID3D11Buffer* CBs[2] = { m_oceanPlane->GetVSCB(), m_ocean->GetCombineWaveCB() };
			context->DSSetConstantBuffers(0, 2, CBs);

			CBs[0] = m_oceanPlane->GetPSCB();
			context->HSSetConstantBuffers(0, 1, CBs);

			m_oceanPlane->Draw(context);

			ID3D11ShaderResourceView* release[6] = { 0, };
			context->DSSetShaderResources(0, 6, release);

			m_deviceResources->PIXEndEvent();
		}
	}
	m_deviceResources->PIXEndEvent();


	m_deviceResources->PIXBeginEvent(L"PostProcess");
	context->OMSetRenderTargets(0, NULL, NULL); // to release texture2D from RTV
	// post process, multiple RTV로 묶어서 postprocess.Process()로 퉁치고싶은데 왜 인자로 넘겨주면 안되고 이렇게 바깥에서해야하는거지?
	{
		context->CopyResource(m_postProcess->GetFirstTexture(), m_floatBuffer.Get());
		m_postProcess->Process(context);

		ID3D11ShaderResourceView* resources[] = {
			m_floatSRV.Get(),
			m_postProcess->GetFirstSRV()
		};
		context->PSSetShaderResources(0, 2, resources);

		auto rtv = m_deviceResources->GetRenderTargetView();
		context->OMSetRenderTargets(1, &rtv, NULL);

		m_postProcess->Draw(context);

		ID3D11ShaderResourceView* nullSRV[6] = { 0, };
		context->PSSetShaderResources(0, 6, nullSRV);
	}
	m_deviceResources->PIXEndEvent();


	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Show the new frame.
	m_deviceResources->Present();


}

// Helper method to clear the back buffers.
void Game::Clear()
{
	m_deviceResources->PIXBeginEvent(L"Clear");

	// Clear the views.
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto depthStencil = m_deviceResources->GetDepthStencilView();

	auto* backBufferRTV = m_deviceResources->GetRenderTargetView();

	context->ClearRenderTargetView(m_floatRTV.Get(), Colors::Black); // HDR Pipeline, using float RTV
	context->ClearRenderTargetView(backBufferRTV, Colors::Black);

	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// for post process
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

	// Init Assets
	{
		// 끔찍하다, 기능구현 다하면 고칠게요
		// Cubemap
		{
			DX::ThrowIfFailed(CreateDDSTextureFromFileEx(device, L"./Assets/IBL/brightEnvHDR.dds", 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, DDS_LOADER_DEFAULT, nullptr, m_cubemapEnvView.GetAddressOf(), nullptr));
			DX::ThrowIfFailed(CreateDDSTextureFromFileEx(device, L"./Assets/IBL/brightBrdf.dds", 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D10_RESOURCE_MISC_FLAG(false), DDS_LOADER_DEFAULT, nullptr, m_cubemapBRDFView.GetAddressOf(), nullptr));
			DX::ThrowIfFailed(CreateDDSTextureFromFileEx(device, L"./Assets/IBL/brightDiffuseHDR.dds", 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, DDS_LOADER_DEFAULT, nullptr, m_cubemapIrradianceView.GetAddressOf(), nullptr));
			DX::ThrowIfFailed(CreateDDSTextureFromFileEx(device, L"./Assets/IBL/brightSpecularHDR.dds", 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, DDS_LOADER_DEFAULT, nullptr, m_cubemapSpecularView.GetAddressOf(), nullptr));

			MeshData cube = GeometryGenerator::MakeBox(100.f);
			std::vector<std::unique_ptr<ModelMeshPart>> cubeMapMeshes;
			cubeMapMeshes.push_back(std::make_unique<ModelMeshPart>(cube, device));

			m_cubeMap = std::make_unique<Model>("cubeMap", std::move(cubeMapMeshes), Graphics::cubemapPSO);
			m_cubeMap->Initialize(device, {});
		}

		// Sample model
		{
			MeshData sphereMesh = GeometryGenerator::MakeSphere(1.f, 100, 100);
			std::vector<std::unique_ptr<ModelMeshPart>> modelMeshes;
			modelMeshes.push_back(std::make_unique<ModelMeshPart>(sphereMesh, device));

			m_models.push_back(std::make_unique<Model>("SAMPLE SPHERE", std::move(modelMeshes), Graphics::basicPSO));

			m_models.back()->Initialize(device, {
				L"./Assets/Textures/worn_shiny/worn-shiny-metal-albedo.png",
				L"./Assets/Textures/worn_shiny/worn-shiny-metal-ao.png",
				L"./Assets/Textures/worn_shiny/worn-shiny-metal-Height.png",
				L"./Assets/Textures/worn_shiny/worn-shiny-metal-Metallic.png",
				L"./Assets/Textures/worn_shiny/worn-shiny-metal-Normal-dx.png",
				L"./Assets/Textures/worn_shiny/worn-shiny-metal-Roughness.png"
				});

			// m_models.back()->UpdatePosBy(DirectX::SimpleMath::Matrix::CreateTranslation(0.f, 5.f, 0.f));
		}

		// Ocean
		{

			m_ocean = std::make_unique<Ocean>(device);
			MeshData quadPatches;
			GeometryGenerator::MakeCWQuadPatches(128, &quadPatches);


			std::vector<std::unique_ptr<ModelMeshPart>> meshes;
			meshes.push_back(std::make_unique<ModelMeshPart>(quadPatches, device));
			m_oceanPlane = std::make_unique<Model>("Tessellated Quad Plane", std::move(meshes), Graphics::Ocean::OceanPSO);

			auto manipulate = Matrix::CreateScale(ocean::WORLD_SCALER);
			manipulate *= Matrix::CreateRotationX(DirectX::XM_PIDIV2);
			m_oceanPlane->UpdatePosByTransform(manipulate);

			m_oceanPlane->Initialize(device, {});
		}

	}

	Utility::DXResource::CreateConstantBuffer(m_lightsConstantsCPU, device, m_lightsConstantBuffer);
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
	auto size = m_deviceResources->GetOutputSize();
	m_proj = Matrix::CreatePerspectiveFieldOfView(
		XMConvertToRadians(90.f),
		float(size.right) / float(size.bottom), 0.1f, 300.f);

	auto* device = m_deviceResources->GetD3DDevice();


	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; // for HDR Pipeline
	desc.Width = size.right;
	desc.Height = size.bottom;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = 0;

	// HDR Pipeline
	DX::ThrowIfFailed(device->CreateTexture2D(&desc, NULL, m_floatBuffer.GetAddressOf()));
	CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R16G16B16A16_FLOAT);
	DX::ThrowIfFailed(device->CreateRenderTargetView(m_floatBuffer.Get(), &renderTargetViewDesc, m_floatRTV.ReleaseAndGetAddressOf()));
	DX::ThrowIfFailed(device->CreateShaderResourceView(m_floatBuffer.Get(), NULL, m_floatSRV.GetAddressOf()));

	m_postProcess = std::make_unique<PostProcess>();
	m_postProcess->Initialize(device, size);

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
