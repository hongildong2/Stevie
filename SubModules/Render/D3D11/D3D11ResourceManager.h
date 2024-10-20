#pragma once
#include "pch.h"
#include "../RenderDefs.h"


class D3D11Texture2D;
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


	D3D11MeshGeometry* CreateMeshGeometry(const void* pInVertexList, const UINT vertexSize, const UINT vertexCount, const void* pInIndexList, const UINT indexSize, const UINT indexCount);
	D3D11Texture2D* CreateTexture2D(const UINT width, const UINT height, const DXGI_FORMAT format);
	D3D11Texture2D* CreateTextureFromFile(const WCHAR* fileName);
	D3D11TextureCube* CreateTextureCubeFromFile(const WCHAR* fileName);

	void CreateConstantBuffer(const UINT bufferSize, const void* pInitData, ID3D11Buffer** ppOutBuffer);
	void UpdateConstantBuffer(const UINT bufferSize, const void* pData, ID3D11Buffer* pBuffer);

	void InitializeCommonResource() const;

private:
	void CreateVertexBuffer(ID3D11Buffer** pOutBuffer, const void* pInVertexList, const UINT vertexSize, const UINT vertexCount);
	void CreateIndexBuffer(ID3D11Buffer** pOutBuffer, const void* pInIndexList, const UINT indexSize, const UINT indexCount);


private:
	D3D11Renderer* m_pRenderer;
	std::unordered_map<std::wstring, IRenderResource*> m_namedResourcesMap;
	std::unordered_set<IRenderResource*> m_unnamedResourcesMap;
};

