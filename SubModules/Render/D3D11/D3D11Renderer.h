#pragma once
#include "../IRenderer.h"
#include "D3D11ResourceManager.h"
#include "D3D11DeviceResources.h"

class RMaterial;

class D3D11Renderer : public IRenderer
{
public:
	D3D11Renderer();
	~D3D11Renderer() = default;
	virtual BOOL Initialize(BOOL bEnableDebugLayer, BOOL bEnableGBV, const WCHAR* wchShaderPath) override;

	virtual void BeginRender() override;
	virtual void EndRender() override;
	virtual void Present() override;

	virtual BOOL SetWindow(HWND hWnd, DWORD dwBackBufferWidth, DWORD dwBackBufferHeight) override;
	virtual BOOL UpdateWindowSize(DWORD dwBackBufferWidth, DWORD dwBackBufferHeight) override;

	// Renders
	virtual void Render(const MeshComponent* pInMeshComponent, DirectX::SimpleMath::Matrix worldRow) override;
	virtual void Compute(const RTexture** pResults, const UINT resultsCount, const RTexture** pResources, const UINT resourcesCount, const RSamplerState** pSamplerStates, const UINT samplerStatesCount, const void** alignedConstants, const UINT** constantSizes, const UINT constantsCount, const UINT batchX, const UINT batchY, const UINT batchZ) override;

	// IRenderDevice
	virtual RTexture* CreateTextureFromFile(const WCHAR* wchFileName) override;
	virtual RTexture* CreateTextureFromDDSFile(const WCHAR* wchFileName) override;
	virtual RTexture* CreateTextureCubeFromFile(const WCHAR* wchFileName) override;
	virtual RTexture* CreateTexture3D(const UINT width, const UINT height, const UINT depth, const DXGI_FORMAT format) override;
	virtual RTexture* CreateDynamicTexture(const UINT width, const UINT height, const DXGI_FORMAT format) override;

	virtual RMeshGeometry* CreateMeshGeometry(const void* pInVertexList, const UINT vertexSize, const UINT vertexCount, const void* pInIndexList, const UINT indexSize, const UINT indexCount) override;
	virtual RMeshGeometry* CreateQuadPatches(const UINT patchCount) override;

	// Scene
	virtual void SetCamera(const Camera* pCamera) override;
	virtual void SetSkybox(Skybox* pSkybox) override;
	virtual void SetIBLTextures(const RTexture* pIrradianceMapTexture, const RTexture* pSpecularMapTexture, const RTexture* pBRDFMapTexture) override;
	virtual void AddLight(const Light* pLight) override;



	inline const D3D11DeviceResources* GetDeviceResources() const
	{
		return m_deviceResources.get();
	}

	inline const WCHAR* GetShaderPath() const
	{
		return m_shaderPath.c_str();
	}

private:
	void SetGlobalConstant();
	void SetMeshConstant(const MeshComponent* pMeshComponent, DirectX::SimpleMath::Matrix worldRow);
	void SetPipelineStateByMaterial(const RMaterial* pMaterial);
private:
	std::unique_ptr <D3D11DeviceResources> m_deviceResources;
	std::unique_ptr <D3D11ResourceManager> m_resourceManager;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_globalCB;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_meshCB;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_materialCB;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_lightCB; // TEMP sun light

	std::wstring m_shaderPath;

	// Scene
	const Camera* m_camera;
	Skybox* m_skybox;

	const D3D11TextureCube* m_irradianceMapTexture;
	const D3D11TextureCube* m_specularMapTexture;
	const D3D11Texture2D* m_BRDFMapTexture;
	std::vector<const Light*> m_lights;



};