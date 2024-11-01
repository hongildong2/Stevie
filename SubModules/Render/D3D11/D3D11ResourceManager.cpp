#include "pch.h"
#include "D3D11Renderer.h"
#include "D3D11Texture.h"
#include "D3D11MeshGeometry.h"
#include "D3D11DeviceResources.h"

#include "D3D11ResourceManager.h"
#include "../GraphicsCommon1.h"
#include "../RenderDefs.h"

D3D11ResourceManager::~D3D11ResourceManager()
{
	Graphics::D3D11::ClearCommonResources();
}

void D3D11ResourceManager::Initialize(D3D11Renderer* pRenderer)
{
	m_pRenderer = pRenderer;

	InitializeCommonResource();
}

void D3D11ResourceManager::CreateVertexBuffer(ID3D11Buffer** pOutBuffer, const void* pInVertexList, const UINT vertexSize, const UINT vertexCount)
{
	auto* pDevice = m_pRenderer->GetDeviceResources()->GetD3DDevice();
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = vertexSize * vertexCount;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = pInVertexList;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	DX::ThrowIfFailed(pDevice->CreateBuffer(&bufferDesc, &InitData, pOutBuffer));



}

void D3D11ResourceManager::CreateIndexBuffer(ID3D11Buffer** pOutBuffer, const void* pInIndexList, const UINT indexSize, const UINT indexCount)
{
	auto* pDevice = m_pRenderer->GetDeviceResources()->GetD3DDevice();

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = indexSize * indexCount;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = pInIndexList;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	DX::ThrowIfFailed(pDevice->CreateBuffer(&bufferDesc, &InitData, pOutBuffer));
}

D3D11MeshGeometry* D3D11ResourceManager::CreateMeshGeometry(const void* pInVertexList, const UINT vertexSize, const UINT vertexCount, const void* pInIndexList, const UINT indexSize, const UINT indexCount)
{
	D3D11MeshGeometry* res = new D3D11MeshGeometry(EPrimitiveTopologyType::TRIANGLE);
	res->Initialize(m_pRenderer, pInVertexList, vertexSize, vertexCount, pInIndexList, indexSize, indexCount);
	return res;
}

D3D11Texture2D* D3D11ResourceManager::CreateTexture2D(const UINT width, const UINT height, const DXGI_FORMAT format)
{
	D3D11Texture2D* res = new D3D11Texture2D();
	res->Initialize(m_pRenderer, width, height, format, FALSE);

	return res;
}

D3D11TextureDepth* D3D11ResourceManager::CreateTextureDepth(const UINT width, const UINT height)
{
	D3D11TextureDepth* res = new D3D11TextureDepth();
	res->Initialize(m_pRenderer, width, height);
	return res;
}

D3D11Texture2D* D3D11ResourceManager::CreateTextureFromFile(const WCHAR* fileName)
{
	D3D11Texture2D* res = new D3D11Texture2D();
	res->InitializeFromFile(m_pRenderer, fileName);
	return res;
}

D3D11Texture2D* D3D11ResourceManager::CreateTextureFromDDSFile(const WCHAR* fileName)
{
	D3D11Texture2D* res = new D3D11Texture2D();
	res->InitializeFromDDSFile(m_pRenderer, fileName);
	return res;
}

D3D11TextureCube* D3D11ResourceManager::CreateTextureCubeFromFile(const WCHAR* fileName)
{
	D3D11TextureCube* res = new D3D11TextureCube();
	res->Initialize(m_pRenderer, fileName);
	return res;
}

void D3D11ResourceManager::CreateConstantBuffer(const UINT bufferSize, const void* pInitData, ID3D11Buffer** ppOutBuffer)
{
	auto* pDevice = m_pRenderer->GetDeviceResources()->GetD3DDevice();

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ByteWidth = bufferSize;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	if (pInitData == nullptr)
	{
		DX::ThrowIfFailed(pDevice->CreateBuffer(&desc, NULL, ppOutBuffer));
	}
	else
	{
		D3D11_SUBRESOURCE_DATA initDataDesc;
		ZeroMemory(&initDataDesc, sizeof(initDataDesc));
		initDataDesc.pSysMem = pInitData;
		initDataDesc.SysMemPitch = 0;
		initDataDesc.SysMemSlicePitch = 0;

		DX::ThrowIfFailed(pDevice->CreateBuffer(&desc, &initDataDesc, ppOutBuffer));
	}

}
void D3D11ResourceManager::UpdateConstantBuffer(const UINT bufferSize, const void* pData, ID3D11Buffer* pBuffer)
{
	auto* pContext = m_pRenderer->GetDeviceResources()->GetD3DDeviceContext();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	DX::ThrowIfFailed(pContext->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

	memcpy(mappedResource.pData, pData, bufferSize);

	pContext->Unmap(pBuffer, 0);
}

void D3D11ResourceManager::CreateStructuredBuffer(const UINT bufferSize, const UINT byteStride, const void* pInitData, ID3D11Buffer** ppOutBuffer)
{
	MY_ASSERT(ppOutBuffer != nullptr);
	auto* pDevice = m_pRenderer->GetDeviceResources()->GetD3DDevice();

	D3D11_BUFFER_DESC sbDesc;
	ZeroMemory(&sbDesc, sizeof(sbDesc));
	sbDesc.ByteWidth = bufferSize;
	sbDesc.Usage = D3D11_USAGE_DYNAMIC;
	sbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	sbDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	sbDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	sbDesc.StructureByteStride = byteStride;

	if (pInitData == nullptr)
	{
		DX::ThrowIfFailed(pDevice->CreateBuffer(&sbDesc, nullptr, ppOutBuffer));
	}
	else
	{
		D3D11_SUBRESOURCE_DATA initData;
		ZeroMemory(&initData, sizeof(initData));
		initData.pSysMem = pInitData;
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		DX::ThrowIfFailed(pDevice->CreateBuffer(&sbDesc, &initData, ppOutBuffer));
	}
}

void D3D11ResourceManager::UpdateStructuredBuffer(const UINT dataSize, const UINT firstElement, const UINT numElements, const void* pData, ID3D11Buffer* pInBuffer, ID3D11ShaderResourceView** ppOutSRV)
{
	auto* pContext = m_pRenderer->GetDeviceResources()->GetD3DDeviceContext();
	auto* pDevice = m_pRenderer->GetDeviceResources()->GetD3DDevice();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	DX::ThrowIfFailed(pContext->Map(pInBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

	memcpy(mappedResource.pData, pData, dataSize);

	pContext->Unmap(pInBuffer, 0);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = firstElement;
	srvDesc.Buffer.NumElements = numElements;
	srvDesc.Buffer.
		// TODO :: Utiity.h Buffer함수 참고

		DX::ThrowIfFailed(pDevice->CreateShaderResourceView(pInBuffer, &srvDesc, ppOutSRV));
}

void D3D11ResourceManager::InitializeCommonResource() const
{
	Graphics::D3D11::InitCommonResources(m_pRenderer);
}
