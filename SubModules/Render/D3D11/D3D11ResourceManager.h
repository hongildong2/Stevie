#pragma once
#include "pch.h"
#include "../RenderDefs.h"

class D3D11Renderer;

class D3D11ResourceManager final
{
public:
	D3D11ResourceManager() = default;
	~D3D11ResourceManager();
	void Initialize(D3D11Renderer* pRenderer);


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

	void CreateConstantBuffer(const UINT bufferSize, const void* pInitData, ID3D11Buffer** ppOutBuffer);
	void UpdateConstantBuffer(const UINT bufferSize, const void* pData, ID3D11Buffer* pBuffer);
	RTexture* CreateStructuredBuffer(const UINT uElementSize, const UINT uCount, const void* pInitData);
	void UpdateStructuredBuffer(const UINT uElementSize, const UINT uElementCount, const void* pData, RTexture* pInBuffer);

	void InitializeCommonResource() const;

private:
	void CreateVertexBuffer(ID3D11Buffer** pOutBuffer, const void* pInVertexList, const UINT vertexSize, const UINT vertexCount);
	void CreateIndexBuffer(ID3D11Buffer** pOutBuffer, const void* pInIndexList, const UINT indexSize, const UINT indexCount);


private:
	D3D11Renderer* m_pRenderer;
};

