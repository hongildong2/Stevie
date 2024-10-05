#pragma once
#include "pch.h"
#include "RenderDefs.h"


using namespace DirectX;
using namespace DX;

class IRenderResource
{
public:
	IRenderResource() = default;
	virtual ~IRenderResource() = default;
	IRenderResource(const IRenderResource& other) = delete;
	IRenderResource& operator=(const IRenderResource& other) = delete;
};


class RTexture2D;
class RTextureVolume;
class RMeshGeometry;
class RMaterial;
class RShader;
class RConstantBuffer;

class MeshComponent;
class OceanMeshComponent;
class CloudMeshComponent;

class IRenderer
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
	virtual void Render(const MeshComponent* pInMeshComponent) = 0;  // opaquea면 바로 depth only pass rendering, add to main render list. tranparent면 목록에 추가. 
	virtual void RenderOcean(const OceanMeshComponent* pInOcean) = 0;
	virtual void RenderCloud(const CloudMeshComponent* pInRender) = 0;

	// Computes
	virtual BOOL ComputeOcean(const OceanMeshComponent* pInOcean) = 0;
	virtual BOOL ComputeCloud(const CloudMeshComponent* pInCloud) = 0;


	// IRenderDevice
	virtual RTexture2D* CreateTextureFromFile(const WCHAR* wchFileName) = 0;
	virtual RTextureVolume* CreateTextureVolume(const UINT width, const UINT height, const UINT depth, const DXGI_FORMAT format) = 0;
	virtual RTexture2D* CreateDynamicTexture(const UINT width, const UINT height, const DXGI_FORMAT format) = 0;

	virtual RMeshGeometry* CreateBasicMeshGeometry(const EBasicGeometry type) = 0;
	virtual RMeshGeometry* CreateMeshGeometry(const void* pInVertexList, const UINT vertexSize, const UINT vertexCount, const void* pInIndexList, const UINT indexSize, const UINT indexCount) = 0;
	virtual RMeshGeometry* CreateQuadPatches(const UINT patchCount) = 0;

protected:
	virtual RShader* CreateShader(const EShaderType type, const WCHAR* name) = 0;
	virtual RConstantBuffer* CreateConstantBuffer(const UINT bufferSize) = 0;
};
