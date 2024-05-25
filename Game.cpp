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
	m_yaw(0)
{
	// for post processing in compute shader
	m_deviceResources = std::make_unique<DX::DeviceResources>(DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_D32_FLOAT, 2, D3D_FEATURE_LEVEL_11_1, DX::DeviceResources::c_EnableHDR);
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

	// TODO : 여기다 대충 dto같은애들 만든 뒤 개체 Update 이용해서 개체 정보 업데이트하기, DTO작업 미리해놓고 나중에 연결해도됨
	// Light DTO

	// Model DTO

	// PS Phong shader parameter DTO

	// PS PBR shader parameter DTO

	//

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
			for (Model& model : m_models)
			{
				snprintf(buf, 20, "%d%s", nth++, "th model");
				if (ImGui::TreeNode(buf))
				{
					Material mat = model.GetMaterialConstant();

					// Vector3 <-> float3는 이렇게
					ImGui::SliderFloat3("diffuse", &mat.diffuse.x, 0.f, 1.f);
					ImGui::SliderFloat3("ambient", &mat.ambient.x, 0.f, 1.f);
					ImGui::SliderFloat3("specular", &mat.specular.x, 0.f, 1.f);
					ImGui::TreePop();

					model.UpdateMaterialConstant(mat);
				}
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("ImageFilter"))
		{
			ImGui::SliderFloat("threshold", &m_postProcessConstant.threshold, 0.f, 5.f);
			ImGui::SliderFloat("strength", &m_postProcessConstant.strength, 0.f, 1.f);
			ImGui::SliderFloat("exposure", &m_postProcessConstant.exposure, 0.f, 3.f);
			ImGui::SliderFloat("gamma", &m_postProcessConstant.gamma, -0.f, 2.f);
			ImGui::SliderFloat("blur", &m_postProcessConstant.blur, 0.f, 10.f);

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


	// Scene.Update() : Update light and etc. CPU info
	{

	}

	elapsedTime;
}
#pragma endregion

#pragma region Frame Render
void Game::SetPipelineState(ID3D11DeviceContext* context, GraphicsPSO& pso)
{
	context->VSSetShader(pso.m_vertexShader.Get(), 0, 0);
	context->PSSetShader(pso.m_pixelShader.Get(), 0, 0);
	context->HSSetShader(pso.m_hullShader.Get(), 0, 0);
	context->DSSetShader(pso.m_domainShader.Get(), 0, 0);
	context->GSSetShader(pso.m_geometryShader.Get(), 0, 0);
	context->CSSetShader(NULL, 0, 0);
	context->IASetInputLayout(pso.m_inputLayout.Get());
	context->RSSetState(pso.m_rasterizerState.Get());
	context->OMSetBlendState(pso.m_blendState.Get(), pso.m_blendFactor,
		0xffffffff);
	context->OMSetDepthStencilState(pso.m_depthStencilState.Get(),
		pso.m_stencilRef);
	context->IASetPrimitiveTopology(pso.m_primitiveTopology);
}

void Game::SetPipelineState(ID3D11DeviceContext* context, ComputePSO& pso)
{
	context->VSSetShader(NULL, 0, 0);
	context->PSSetShader(NULL, 0, 0);
	context->HSSetShader(NULL, 0, 0);
	context->DSSetShader(NULL, 0, 0);
	context->GSSetShader(NULL, 0, 0);
	context->CSSetShader(pso.m_computeShader.Get(), 0, 0);
}

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

	// if PostProcess on
	if (true)
	{
		// keep previously setted rtv, add this one 이거 왜 안될까?? 텍스쳐에 RTV로 렌더하게 했는데 그림이 안그려져서 검은화면만나옴
		// context->OMSetRenderTargetsAndUnorderedAccessViews(1, m_frontPostProcessRTV.GetAddressOf(), NULL, 0, 0, NULL, 0);
	}
	// Scene.Draw()
	{
		// Light.Draw()
		Utility::DXResource::UpdateConstantBuffer(m_lightsConstantsCPU, context, m_lightsConstantBuffer);
		context->PSSetConstantBuffers(1, 1, m_lightsConstantBuffer.GetAddressOf());

		const Vector3 eyePos = m_camera->GetEyePos(); // 지금은 이것만

		// IBL을 위해 0번에 큐브맵 텍스쳐 고정
		context->PSSetShaderResources(0, 1, m_cubemapTextureView.GetAddressOf());
		context->PSSetSamplers(0, 1, Graphics::linearWrapSS.GetAddressOf());

		SetPipelineState(context, Graphics::cubemapPSO);
		m_cubeMap->PrepareForRendering(context, viewMatrix, m_proj, eyePos);
		m_cubeMap->Draw(context);


		SetPipelineState(context, Graphics::basicPSO);
		for (Model& model : m_models)
		{
			model.PrepareForRendering(context, viewMatrix, m_proj, eyePos);
			model.Draw(context);
		}
	}

	// PostProcess.Draw()
	if (true)
	{
		// TODO : 필터 자유자재로 넣고 뺄 수 있게 front, back 버퍼들 바꿔가면서 하는거 벡터이용해서 일반화 하기
		// 백버퍼 복사 -> 필터 1 -> 필터 2 -> 버퍼2에 최종결과 라면 버퍼1에 백버퍼 복사 -> 버퍼1, 버퍼2 SRV로 해서 FilterCombine PS
		// Gaussian Blur
		// front buffer has rendering result and MUST have final processed result also
		const RECT outputSize = m_deviceResources->GetOutputSize();

		// release backbuffer RTV for postprocess to use this as resource
		// context->OMSetRenderTargets(0, NULL, NULL);
		ComPtr<ID3D11Texture2D> backbuffer;
		DX::ThrowIfFailed(m_deviceResources->GetSwapChain()->GetBuffer(0, IID_PPV_ARGS(backbuffer.GetAddressOf())));

		context->CopyResource(m_frontPostProcessTextureBuffer.Get(), backbuffer.Get());
		// total 1024 thread groups. 화면을 가로 32 세로 32의 스레드 그룹으로 나눠서 작업을 분배한다
		// 어느 픽셀에 위치해있는지, 어딜 참조해야 하는지지는 CS에서 DispathchThreadID가 대응
		// TODO : UpSample, DownSample Blur, Faster Image Processing
		const unsigned int GROUP_X = ceil(outputSize.right / 32.f);
		const unsigned int GROUP_Y = ceil(outputSize.bottom / 32.f);
		for (int i = 0; i < 50; ++i)
		{
			// X
			SetPipelineState(context, Graphics::blurXPSO);
			context->CSSetShaderResources(0, 1, m_frontPostProcessSRV.GetAddressOf());
			context->CSSetUnorderedAccessViews(0, 1, m_backPostProcessUAV.GetAddressOf(), NULL);

			context->Dispatch(GROUP_X, GROUP_Y, 1);

			Utility::ComputeShaderBarrier(context);

			// Y
			SetPipelineState(context, Graphics::blurYPSO);
			context->CSSetShaderResources(0, 1, m_backPostProcessSRV.GetAddressOf());
			context->CSSetUnorderedAccessViews(0, 1, m_frontPostProcessUAV.GetAddressOf(), NULL);

			context->Dispatch(GROUP_X, GROUP_Y, 1);
			Utility::ComputeShaderBarrier(context);
		}

		// 백버퍼에 있는 렌더 결과와 가우시안 블러 결과 합쳐서 블룸 구현
		context->CopyResource(m_backPostProcessTextureBuffer.Get(), backbuffer.Get());
		// 이제 front에 블러, back에 원래 화면 저장..되어있음 그걸 Combine해서 다시 그리면 된다. 하는김에 톤맵핑도 같이

		SetPipelineState(context, Graphics::filterCombinePSO);
		// 0번에 원본, 1번에 블러 지정
		ID3D11ShaderResourceView* resources[] = {
			 m_backPostProcessSRV.Get(), m_frontPostProcessSRV.Get()
		};
		Utility::DXResource::UpdateConstantBuffer(m_postProcessConstant, context, m_postProcessCB);
		context->PSSetShaderResources(0, 2, resources);
		context->PSSetConstantBuffers(0, 1, m_postProcessCB.GetAddressOf());
		m_screenQuad->Draw(context);
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


	// Texture2Ds For GaussianBlur and Tonemapping
	{
		const DXGI_FORMAT backbufferforamt = m_deviceResources->GetBackBufferFormat();
		const RECT outputSize = m_deviceResources->GetOutputSize();
		// description for UAV
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = backbufferforamt;
		desc.Width = outputSize.right;
		desc.Height = outputSize.bottom;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS;
		desc.MiscFlags = 0;
		desc.CPUAccessFlags = 0;

		DX::ThrowIfFailed(device->CreateTexture2D(&desc, NULL, m_backPostProcessTextureBuffer.GetAddressOf()));
		DX::ThrowIfFailed(device->CreateTexture2D(&desc, NULL, m_frontPostProcessTextureBuffer.GetAddressOf()));

		DX::ThrowIfFailed(device->CreateShaderResourceView(m_backPostProcessTextureBuffer.Get(), NULL, m_backPostProcessSRV.GetAddressOf()));
		DX::ThrowIfFailed(device->CreateUnorderedAccessView(m_backPostProcessTextureBuffer.Get(), NULL, m_backPostProcessUAV.GetAddressOf()));

		DX::ThrowIfFailed(device->CreateShaderResourceView(m_frontPostProcessTextureBuffer.Get(), NULL, m_frontPostProcessSRV.GetAddressOf()));
		DX::ThrowIfFailed(device->CreateUnorderedAccessView(m_frontPostProcessTextureBuffer.Get(), NULL, m_frontPostProcessUAV.GetAddressOf()));

		Utility::DXResource::CreateConstantBuffer(m_postProcessConstant, device, m_postProcessCB);
	}

	// Texture
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(device, L"earth.bmp", nullptr,
			textureView.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(CreateDDSTextureFromFileEx(device, L"skybox.dds", 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, DDS_LOADER_FLAGS(false), m_cubemapTexture.GetAddressOf(), m_cubemapTextureView.ReleaseAndGetAddressOf(), nullptr));



	// Init Assets
	{

		std::vector<ModelMeshPart> meshes;

		// cubemap Init
		MeshData cube = GeometryGenerator::MakeBox(50.f);
		ModelMeshPart cubeMesh = ModelMeshPart(cube, device);
		meshes.push_back(cubeMesh);
		m_cubeMap = std::make_unique<Model>("cubeMap", meshes, Vector3(0.f, 0.f, 0.f));
		m_cubeMap->Initialize(device, m_cubemapTextureView);
		meshes.clear();

		MeshData&& sphereMesh = GeometryGenerator::MakeSphere(1.f, 100, 100);
		ModelMeshPart mesh = ModelMeshPart(sphereMesh, device);
		meshes.push_back(mesh);
		Model&& sphereModel = Model("BASIC SPHERE", meshes, DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f));
		sphereModel.Initialize(device, textureView);
		m_models.push_back(sphereModel);
		meshes.clear();

		MeshData quad = GeometryGenerator::MakeSquare();
		ModelMeshPart quadMesh = ModelMeshPart(quad, device);
		meshes.push_back(quadMesh);
		m_screenQuad = std::make_unique<Model>("Scren Quad", meshes, Vector3(0.f, 0.f, 0.f));
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
