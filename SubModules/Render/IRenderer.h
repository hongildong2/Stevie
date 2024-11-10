#pragma once
#include "pch.h"


using namespace DirectX;

class RTexture;
class RMeshGeometry;
class RShader;
class RConstantBuffer;
class RSamplerState;
class Camera;
class Skybox;
class Light;

class MeshComponent;

interface IRenderer
{
public:
	IRenderer() = default;
	virtual ~IRenderer() = default;

	IRenderer(const IRenderer& other) = delete;
	IRenderer operator=(const IRenderer& other) = delete;

	IRenderer(const IRenderer&& other) = delete;
	IRenderer operator=(const IRenderer&& other) = delete;

	virtual BOOL Initialize(BOOL bEnableDebugLayer, BOOL bEnableGBV, const WCHAR* wchShaderPath) = 0;

	virtual void BeginRender() = 0;
	virtual void Render() = 0;
	virtual void EndRender() = 0;
	virtual void Present() = 0;

	virtual BOOL SetWindow(HWND hWnd, DWORD dwBackBufferWidth, DWORD dwBackBufferHeight) = 0;
	virtual BOOL UpdateWindowSize(DWORD dwBackBufferWidth, DWORD dwBackBufferHeight) = 0;

	// Renders
	virtual void Submit(const MeshComponent* pInMeshComponent, DirectX::SimpleMath::Matrix worldRow) = 0;


	// TODO :: Buffer? We need RBuffer to control parameters in game layer
	virtual void Compute(const RTexture** pResults, const UINT resultsCount, const RTexture** pResources, const UINT resourcesCount, const RSamplerState** pSamplerStates, const UINT samplerStatesCount, const void** alignedConstants, const UINT** constantSizes, const UINT constantsCount, const UINT batchX, const UINT batchY, const UINT batchZ) = 0;


	// IRenderDevice
	virtual RTexture* CreateTexture2DFromWICFile(const WCHAR* wchFileName) = 0;
	virtual RTexture* CreateTexture2DFromDDSFile(const WCHAR* wchFileName) = 0;
	virtual RTexture* CreateTextureCubeFromDDSFile(const WCHAR* wchFileName) = 0;
	virtual RTexture* CreateTexture3D(const UINT width, const UINT height, const UINT depth, const DXGI_FORMAT format) = 0;
	virtual RTexture* CreateDynamicTexture(const UINT width, const UINT height, const DXGI_FORMAT format) = 0;

	virtual RMeshGeometry* CreateMeshGeometry(const void* pInVertexList, const UINT vertexSize, const UINT vertexCount, const void* pInIndexList, const UINT indexSize, const UINT indexCount) = 0;
	virtual RMeshGeometry* CreateQuadPatches(const UINT patchCount) = 0;

	// Scene
	virtual void SetCamera(const Camera* pCamera) = 0;
	virtual void SetSkybox(Skybox* pSkybox) = 0;
	virtual void SetSunLight(const Light* pLight) = 0;
	virtual void SetIBLTextures(const RTexture* pIrradianceMapTexture, const RTexture* pSpecularMapTexture, const RTexture* pBRDFMapTexture) = 0;
	virtual void AddLight(const Light* pLight) = 0;
};
