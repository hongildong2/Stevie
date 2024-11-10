#pragma once
#include "../IRenderer.h"
#include "../RenderItem.h"
#include "D3D11ResourceManager.h"
#include "D3D11DeviceResources.h"
#include "D3D11PostProcess.h"

class RMaterial;
class SSceneObject;

class D3D11Renderer : public IRenderer
{
public:
	D3D11Renderer();
	~D3D11Renderer() = default;
	virtual BOOL Initialize(BOOL bEnableDebugLayer, BOOL bEnableGBV, const WCHAR* wchShaderPath) override;

	virtual void BeginRender() override;
	virtual void Render() override;
	virtual void EndRender() override;
	virtual void Present() override;

	virtual BOOL SetWindow(HWND hWnd, DWORD dwBackBufferWidth, DWORD dwBackBufferHeight) override;
	virtual BOOL UpdateWindowSize(DWORD dwBackBufferWidth, DWORD dwBackBufferHeight) override;

	// Renders
	virtual void Submit(const MeshComponent* pInMeshComponent, DirectX::SimpleMath::Matrix worldRow) override;
	virtual void Compute(const RTexture** pResults, const UINT resultsCount, const RTexture** pResources, const UINT resourcesCount, const RSamplerState** pSamplerStates, const UINT samplerStatesCount, const void** alignedConstants, const UINT** constantSizes, const UINT constantsCount, const UINT batchX, const UINT batchY, const UINT batchZ) override;

	// IRenderDevice
	virtual RTexture* CreateTexture2DFromWICFile(const WCHAR* wchFileName) override;
	virtual RTexture* CreateTexture2DFromDDSFile(const WCHAR* wchFileName) override;
	virtual RTexture* CreateTextureCubeFromDDSFile(const WCHAR* wchFileName) override;
	virtual RTexture* CreateTexture3D(const UINT width, const UINT height, const UINT depth, const DXGI_FORMAT format) override;
	virtual RTexture* CreateDynamicTexture(const UINT width, const UINT height, const DXGI_FORMAT format) override;

	virtual RMeshGeometry* CreateMeshGeometry(const void* pInVertexList, const UINT vertexSize, const UINT vertexCount, const void* pInIndexList, const UINT indexSize, const UINT indexCount) override;
	virtual RMeshGeometry* CreateQuadPatches(const UINT patchCount) override;

	// Scene
	virtual void SetCamera(const Camera* pCamera) override;
	virtual void SetSkybox(Skybox* pSkybox) override;
	virtual void SetSunLight(const Light* pLight) override;
	virtual void SetIBLTextures(const RTexture* pIrradianceMapTexture, const RTexture* pSpecularMapTexture, const RTexture* pBRDFMapTexture) override;
	virtual void AddLight(const Light* pLight) override;



	inline const D3D11DeviceResources* GetDeviceResources() const
	{
		return m_deviceResources.get();
	}

	inline D3D11ResourceManager* GetResourceManager() const
	{
		return m_resourceManager.get();
	}

	inline const WCHAR* GetShaderPath() const
	{
		return m_shaderPath.c_str();
	}

	inline const UINT GetBackBufferWidth() const
	{
		return m_dwBackBufferWidth;
	}

	inline const UINT GetBackBufferHeight() const
	{
		return m_dwBackBufferHeight;
	}

private:
	void UpdateGlobalConstant();
	void SetPipelineStateByMaterial(const RMaterial* pMaterial);

	void RenderSkybox();
	// void RenderDepthMap();
	// void RenderShadowMap();
	void RenderOpaques();
	void RenderTransparent();



private:
	DWORD m_dwBackBufferWidth;
	DWORD m_dwBackBufferHeight;

	std::unique_ptr <D3D11DeviceResources> m_deviceResources;
	std::unique_ptr <D3D11ResourceManager> m_resourceManager;
	std::unique_ptr <D3D11PostProcess> m_postProcess;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_globalCB;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_sunLightCB;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_meshCB;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_materialCB;

	std::unique_ptr <D3D11TextureRender> m_HDRRenderTarget;

	std::wstring m_shaderPath;

	// Scene
	static constexpr UINT MAX_RENDER_ITEM = 3000;
	RenderItem m_renderItems[MAX_RENDER_ITEM];
	UINT m_renderItemIndex;

	const Camera* m_camera;
	Skybox* m_skybox;

	// IBL
	const D3D11TextureCube* m_irradianceMapTexture;
	const D3D11TextureCube* m_specularMapTexture;
	const D3D11Texture2D* m_BRDFMapTexture;

	// Sun Light
	const Light* m_sunLight;
	const D3D11TextureDepth* m_sunShadowMap; // TODO :: Manage Depth textures by manager, pooling

	// Dynamic Lights
	static constexpr UINT MAX_SCENE_LIGHTS_COUNT = 500;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_lightsSB; // structured buffer
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_lightsSRV;
	std::vector<const Light*> m_lights;
	// std::vector<const D3D11TextureDepth*> m_shadowMaps;


};