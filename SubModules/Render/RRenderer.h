#pragma once
#include "pch.h"
#include "RenderItem.h"
#include "RenderDefs.h"
#include "RResourceManager.h"
#include "RPostProcess.h"

#ifdef API_D3D11
#define RHIRenderer D3D11Renderer
#include "SubModules/Render/D3D11/D3D11Renderer.h"
#endif

class RResourceManager;
class D3D11DeviceResources;
class RPostProcess;
enum class EPrimitiveTopologyType;
enum class EMeshType;
class RTexture;
class RMeshGeometry;
class RShader;
class RConstantBuffer;
class RSamplerState;
class Camera;
class Skybox;
class Light;
class MeshComponent;

class RRenderer final : public RHIRenderer
{
public:
	RRenderer();
	~RRenderer() = default; // TODO :: RELEASE ALL RESOURCE

	RRenderer(const RRenderer& other) = delete;
	RRenderer operator=(const RRenderer& other) = delete;

	RRenderer(const RRenderer&& other) = delete;
	RRenderer operator=(const RRenderer&& other) = delete;

	BOOL Initialize(BOOL bEnableDebugLayer, BOOL bEnableGBV, const WCHAR* wchShaderPath);

	void BeginRender();
	void Render();
	void EndRender();
	void Present();

	BOOL SetWindow(HWND hWnd, DWORD dwBackBufferWidth, DWORD dwBackBufferHeight);
	BOOL UpdateWindowSize(DWORD dwBackBufferWidth, DWORD dwBackBufferHeight);

	// Renders
	void Submit(const MeshComponent* pInMeshComponent, DirectX::SimpleMath::Matrix worldRow);
	void Compute(const RComputeShader* pComputeShader, const WCHAR* pTaskName, const RTexture** pResults, const UINT resultsCount, const RTexture** pResources, const UINT resourcesCount, const RSamplerState** pSamplerStates, const UINT samplerStatesCount, const RenderParam* pAlignedComputeParam, const UINT batchX, const UINT batchY, const UINT batchZ);


	// IRenderDevice
	RTexture* CreateTexture2DFromWICFile(const WCHAR* wchFileName);
	RTexture* CreateTexture2DFromDDSFile(const WCHAR* wchFileName);
	RTexture* CreateTextureCubeFromDDSFile(const WCHAR* wchFileName);
	RTexture* CreateTexture3D(const UINT width, const UINT height, const UINT depth, const DXGI_FORMAT format);
	RTexture* CreateTexture2D(const UINT width, const UINT height, const UINT count, const DXGI_FORMAT format);
	RTexture* CreateStructuredBuffer(const UINT uElementSize, const UINT uElementCount, const void* pInitData);

	RMeshGeometry* CreateMeshGeometry(const void* pInVertexList, const UINT vertexSize, const UINT vertexCount, const void* pInIndexList, const UINT indexSize, const UINT indexCount, const EPrimitiveTopologyType topologyType, const EMeshType meshType);
	RMeshGeometry* CreateBasicMeshGeometry(EBasicMeshGeometry type);

	// Scene
	void SetCamera(const Camera* pCamera);
	void SetSkybox(Skybox* pSkybox);
	void SetSunLight(const Light* pLight);
	void SetIBLTextures(const RTexture* pIrradianceMapTexture, const RTexture* pSpecularMapTexture, const RTexture* pBRDFMapTexture);
	void AddLight(const Light* pLight);

	void Draw(const RenderItem& renderItem);
	void DrawTessellatedQuad(const RenderItem& renderItem);
	void DrawScreenQuad(const RPixelShader* pPS, const RTexture** ppPSResource, const UINT PSResourceCount, const RSamplerState** ppPSSamplerStates, const UINT PSSamplerCount, const RTexture* pTextureRender, RenderParam* pRenderParam);

	inline RResourceManager* GetResourceManager() const
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

	const RTexture* GetRenderTexture() const;
	const RTexture* GetDepthTexture() const;
protected:
	void UpdateGlobalConstant();
	void SetPipelineState(const RenderItem& item);

	void RenderSkybox();
	void RenderOpaques();
	void RenderTransparent();




private:
	DWORD m_dwBackBufferWidth;
	DWORD m_dwBackBufferHeight;

	std::unique_ptr <RResourceManager> m_resourceManager;
	std::unique_ptr <RPostProcess> m_postProcess;

	std::unique_ptr<RBuffer> m_globalCB;
	std::unique_ptr<RBuffer> m_sunLightCB;

	std::unique_ptr<RBuffer> m_meshCB;
	std::unique_ptr<RBuffer> m_materialCB;
	std::unique_ptr<RBuffer> m_computeCB;

	std::unique_ptr <RTexture> m_HDRRenderTarget;
	std::unique_ptr<RMeshGeometry> m_screenQuad;


	std::wstring m_shaderPath;

	// Scene
	static constexpr UINT MAX_COMPUTE_RESOURCE_COUNT = 20;
	static constexpr UINT MAX_RENDER_ITEM = 8000;
	RenderItem m_renderItems[MAX_RENDER_ITEM];
	UINT m_renderItemIndex;

	const Camera* m_camera;
	Skybox* m_skybox;

	// IBL
	static constexpr UINT SCENE_RESOURCES_COUNT = 4;
	const RTexture* m_irradianceMapTexture;
	const RTexture* m_specularMapTexture;
	const RTexture* m_BRDFMapTexture;
	RTexture* m_sceneLightsBuffer;

	static constexpr UINT MAX_SCENE_LIGHTS_COUNT = 100;
	RLightConstant m_sceneLights[MAX_SCENE_LIGHTS_COUNT];
	UINT m_sceneLightsIndex;

	static constexpr UINT MAX_SHADOWING_LIGHTS_COUNT = 10;
	UINT m_shadowingLightsIndices[MAX_SHADOWING_LIGHTS_COUNT];
	UINT m_shadowingLightsIndex;

	// Sun Light
	const Light* m_sunLight;
};
