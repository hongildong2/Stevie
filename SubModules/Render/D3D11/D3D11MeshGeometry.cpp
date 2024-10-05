#include "pch.h"
#include "D3D11MeshGeometry.h"
#include "D3D11Renderer.h"

D3D11MeshGeometry::D3D11MeshGeometry(const EPrimitiveTopologyType type)
	: RMeshGeometry(type)
{
}

void D3D11MeshGeometry::Initialize(const D3D11Renderer* pRenderer, const void* pInVertexList, const UINT vertexSize, const UINT vertexCount, const void* pInIndexList, const UINT indexSize, const UINT indexCount)
{
	if (!pRenderer)
	{
		// assert;
	}

	m_pRenderer = pRenderer;

	auto* pDevice = m_pRenderer->GetDeviceResources()->GetD3DDevice();
	m_vertexStride = vertexSize;
	m_vertexOffset = 0;
	m_indexCount = indexCount;
	m_indexFormat = indexSize == 32 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
	// assert indexsize 32 or 16

	// init vertex buffer
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
	DX::ThrowIfFailed(pDevice->CreateBuffer(&bufferDesc, &InitData, m_vertexBuffer.GetAddressOf()));

	// index buffer
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = indexSize * indexCount;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = pInIndexList;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	DX::ThrowIfFailed(pDevice->CreateBuffer(&bufferDesc, &InitData, m_indexBuffer.GetAddressOf()));


}

void D3D11MeshGeometry::Draw(const XMMATRIX* pWorld)
{
	// assert
	auto* pContext = m_pRenderer->GetDeviceResources()->GetD3DDeviceContext();
	ID3D11Buffer* meshCBs[1] = { m_meshCB.Get() };
	ID3D11Buffer* vBuffers[1] = { m_vertexBuffer.Get() };

	pContext->IASetVertexBuffers(0, 1, vBuffers, &m_vertexStride, &m_vertexOffset);
	pContext->IASetIndexBuffer(m_indexBuffer.Get(), m_indexFormat, 0);


	// TODO :: update constant buffer
	pContext->VSSetConstantBuffers(1, 2, meshCBs);
	pContext->PSSetConstantBuffers(1, 2, meshCBs);


	ID3D11ShaderResourceView* release[8] = { NULL, };

	pContext->DrawIndexed(m_indexCount, 0, 0);
}
