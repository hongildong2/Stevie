#pragma once
#include "pch.h"
#include "../RenderDefs.h"


class D3D11StructuredBuffer;
class D3D11Texture2D;
class D3D11Texture3D;
class D3D11TextureDepth;
class D3D11TextureRender;
class D3D11TextureCube;
class D3D11TextureVolume;
class D3D11MeshGeometry;
class D3D11Renderer;

class D3D11ResourceManager final
{
public:
	D3D11ResourceManager() = default;
	~D3D11ResourceManager();
	void Initialize(D3D11Renderer* pRenderer);


	D3D11MeshGeometry* CreateMeshGeometry(const void* pInVertexList, const UINT vertexSize, const UINT vertexCount, const void* pInIndexList, const UINT indexSize, const UINT indexCount, const EPrimitiveTopologyType topologyType, const EMeshType meshType);
	D3D11Texture2D* CreateTexture2D(const UINT width, const UINT height, const UINT count, const DXGI_FORMAT format);
	D3D11Texture3D* CreateTexture3D(const UINT width, const UINT height, const UINT depth, const DXGI_FORMAT format);
	D3D11TextureDepth* CreateTextureDepth(const UINT width, const UINT height);
	D3D11TextureRender* CreateTextureRender(const DXGI_FORMAT format, const UINT width, const UINT height);

	D3D11Texture2D* CreateTexture2DFromWICFile(const WCHAR* wchFileName);
	D3D11Texture2D* CreateTexture2DFromDDSFile(const WCHAR* fileName);
	D3D11TextureCube* CreateTextureCubeFromDDSFile(const WCHAR* fileName);

	D3D11MeshGeometry* CreateCube();
	D3D11MeshGeometry* CreateSphere();
	D3D11MeshGeometry* CreateQuad();
	D3D11MeshGeometry* CreateTessellatedQuad();

	void CreateConstantBuffer(const UINT bufferSize, const void* pInitData, ID3D11Buffer** ppOutBuffer);
	void UpdateConstantBuffer(const UINT bufferSize, const void* pData, ID3D11Buffer* pBuffer);
	D3D11StructuredBuffer* CreateStructuredBuffer(const UINT uElementSize, const UINT uCount, const void* pInitData);
	void UpdateStructuredBuffer(const UINT uElementSize, const UINT uCount, const void* pData, D3D11StructuredBuffer* pInBuffer);

	void InitializeCommonResource() const;

private:
	void CreateVertexBuffer(ID3D11Buffer** pOutBuffer, const void* pInVertexList, const UINT vertexSize, const UINT vertexCount);
	void CreateIndexBuffer(ID3D11Buffer** pOutBuffer, const void* pInIndexList, const UINT indexSize, const UINT indexCount);


private:
	D3D11Renderer* m_pRenderer;
};

