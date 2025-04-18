#pragma once
#include "pch.h"

class RRenderer;
class RBuffer;
class RTexture;
class RMeshGeometry;
enum class EMeshType;

class RResourceManager final
{
public:
	RResourceManager() = default;
	~RResourceManager();
	void Initialize(RRenderer* pRenderer);


	RMeshGeometry* CreateMeshGeometry(const void* pInVertexList, const UINT vertexSize, const UINT vertexCount, const void* pInIndexList, const UINT indexSize, const UINT indexCount, const EPrimitiveTopologyType topologyType, const EMeshType meshType);
	RTexture* CreateTexture2D(const UINT width, const UINT height, const UINT count, const DXGI_FORMAT format);
	RTexture* CreateTexture3D(const UINT width, const UINT height, const UINT depth, const DXGI_FORMAT format);
	RTexture* CreateTextureDepth(const UINT width, const UINT height);
	RTexture* CreateTextureRender(const DXGI_FORMAT format, const UINT width, const UINT height);

	RTexture* CreateTexture2DFromWICFile(const WCHAR* wchFileName);
	RTexture* CreateTexture2DFromDDSFile(const WCHAR* fileName);
	RTexture* CreateTextureCubeFromDDSFile(const WCHAR* fileName);

	RMeshGeometry* CreateCube();
	RMeshGeometry* CreateSphere();
	RMeshGeometry* CreateQuad();
	RMeshGeometry* CreateTessellatedQuad();

	void CreateConstantBuffer(const UINT bufferSize, const void* pInitData, RBuffer* pOutBuffer);
	void UpdateConstantBuffer(const UINT bufferSize, const void* pData, const RBuffer* pInBuffer);
	RTexture* CreateStructuredBuffer(const UINT uElementSize, const UINT uCount, const void* pInitData);
	void UpdateStructuredBuffer(const UINT uElementSize, const UINT uElementCount, const void* pData, RTexture* pInBuffer);

	void InitializeCommonResource() const;

private:
	void CreateVertexBuffer(RBuffer* pOutBuffer, const void* pInVertexList, const UINT vertexSize, const UINT vertexCount);
	void CreateIndexBuffer(RBuffer* pOutBuffer, const void* pInIndexList, const UINT indexSize, const UINT indexCount);


private:
	RRenderer* m_pRenderer;
};

