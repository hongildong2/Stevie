//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "Camera.h"

// ¿œ¥‹..
struct Light
{
	DirectX::SimpleMath::Vector3 strength = DirectX::SimpleMath::Vector3(1.0f);              // 12
	float fallOffStart = 0.0f;                     // 4
	DirectX::SimpleMath::Vector3 direction = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f); // 12
	float fallOffEnd = 10.0f;                      // 4
	DirectX::SimpleMath::Vector3 position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -2.0f); // 12
	float spotPower = 100.0f;                        // 4
};
struct Material
{
	DirectX::SimpleMath::Vector3 ambient = DirectX::SimpleMath::Vector3(0.0f);  // 12
	float shininess = 0.01f;           // 4
	DirectX::SimpleMath::Vector3 diffuse = DirectX::SimpleMath::Vector3(0.0f);  // 12
	float dummy1;                     // 4
	DirectX::SimpleMath::Vector3 specular = DirectX::SimpleMath::Vector3(1.0f); // 12
	float dummy2;                     // 4
	DirectX::SimpleMath::Vector3 fresnelR0 = DirectX::SimpleMath::Vector3(1.0f, 0.71f, 0.29f); // Gold
	float dummy3;
};

struct PSConstants
{
	DirectX::SimpleMath::Vector3 eyeWorld;         // 12
	bool useTexture;          // 4
	Material material;        // 48
	DirectX::SimpleMath::Vector3 rimColor = DirectX::SimpleMath::Vector3(1.0f);
	float rimPower;
	float rimStrength = 0.0f;
	bool useSmoothstep = false;
	float dummy[2];
};
struct LightConstants
{
	Light dirLight;
	Light pointLight;
	Light spotLight;
};



// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:

	Game() noexcept(false);
	~Game() = default; // TODO : destroy cubemap

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

	void Clear();

	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();

	// Device resources.
	std::unique_ptr<DX::DeviceResources>    m_deviceResources;

	// Rendering loop timer.
	DX::StepTimer                           m_timer;


	// Scene
	std::vector<Model> m_models;
	Camera m_camera;
	std::unique_ptr<Model> m_cubeMap;

	PSConstants m_PSConstantsCPU;
	LightConstants m_lightsConstantsCPU;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_lightsConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_PSConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureView;
	Microsoft::WRL::ComPtr<ID3D11Resource> m_texture;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cubemapTextureView;
	Microsoft::WRL::ComPtr<ID3D11Resource> m_cubemapTexture;
};
