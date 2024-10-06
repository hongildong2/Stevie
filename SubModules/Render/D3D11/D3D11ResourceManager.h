#pragma once
#include "pch.h"
#include "../RenderDefs.h"

class D3D11Shader;
class D3D11VertexShader;
class D3D11PixelShader;
class D3D11DomainShader;
class D3D11HullShader;
class D3D11ComputeShader;
class D3D11BlendState;
class D3D11DepthStencilState;
class D3D11InputLayout;
class D3D11SamplerState;
class D3D11RasterizerState;
class D3D11Texture2D;
class D3D11TextureVolume;
class D3D11ConstantBuffer;

class D3D11ResourceManager final
{
public:
	void Initialize(ID3D11Device1* pDevice);

	void CreateVertexBuffer(ID3D11Buffer** pOutBuffer, const void* pInVertexList, const UINT vertexSize, const UINT vertexCount);
	void CreateIndexBuffer(ID3D11Buffer** pOutBuffer, const void* pInIndexList, const UINT indexSize, const UINT indexCount);
	D3D11Texture2D* CreateTexture2D(const UINT width, const UINT height, const DXGI_FORMAT format);
	D3D11Texture2D* CreateTextureFromFile(const WCHAR* fileName);
	D3D11ConstantBuffer* CreateConstantBuffer(const UINT bufferSize);
	void UpdateConstantBuffer(D3D11ConstantBuffer* pInBuffer);
	void InitializeCommonResource();


private:
	ID3D11Device1* m_pDevice;
	std::unordered_map<std::wstring, IRenderResource*> m_namedResourcesMap;
	std::unordered_set<IRenderResource*> m_unnamedResourcesMap;
};

