#pragma once
#include "pch.h"


using namespace DirectX;

class RTexture;
class RTexture2D;
class RTexture3D;
class RMeshGeometry;
class RMaterial;
class RShader;
class RConstantBuffer;
class RSamplerState;


enum class EBasicGeometry;
enum class EShaderType;

class MeshComponent;
class OceanMeshComponent;
class CloudMeshComponent;

interface IRenderer
{
public:
	IRenderer() = default;

	IRenderer(const IRenderer& other) = delete;
	IRenderer operator=(const IRenderer& other) = delete;

	IRenderer(const IRenderer&& other) = delete;
	IRenderer operator=(const IRenderer&& other) = delete;

	virtual BOOL Initialize(BOOL bEnableDebugLayer, BOOL bEnableGBV, const WCHAR* wchShaderPath) = 0;

	virtual void BeginRender() = 0;
	virtual void EndRender() = 0;
	virtual void Present() = 0;

	virtual BOOL SetWindow(HWND hWnd, DWORD dwBackBufferWidth, DWORD dwBackBufferHeight) = 0;
	virtual BOOL UpdateWindowSize(DWORD dwBackBufferWidth, DWORD dwBackBufferHeight) = 0;

	// Renders
	virtual void Render(const MeshComponent* pInMeshComponent) = 0;
	virtual void RenderOcean(const OceanMeshComponent* pInOcean) = 0;
	virtual void RenderCloud(const CloudMeshComponent* pInRender) = 0;


	// TODO :: Buffer? We need RBuffer to control parameters in game layer
	virtual void Compute(const RTexture** pResults, const UINT resultsCount, const RTexture** pResources, const UINT resourcesCount, const RSamplerState** pSamplerStates, const UINT samplerStatesCount, const void** alignedConstants, const UINT** constantSizes, const UINT constantsCount, const UINT batchX, const UINT batchY, const UINT batchZ) = 0;


	// IRenderDevice
	virtual RTexture2D* CreateTextureFromFile(const WCHAR* wchFileName) = 0;
	virtual RTexture3D* CreateTexture3D(const UINT width, const UINT height, const UINT depth, const DXGI_FORMAT format) = 0;
	virtual RTexture2D* CreateDynamicTexture(const UINT width, const UINT height, const DXGI_FORMAT format) = 0;

	virtual RMeshGeometry* CreateMeshGeometry(const void* pInVertexList, const UINT vertexSize, const UINT vertexCount, const void* pInIndexList, const UINT indexSize, const UINT indexCount) = 0;
	virtual RMeshGeometry* CreateQuadPatches(const UINT patchCount) = 0;
};
