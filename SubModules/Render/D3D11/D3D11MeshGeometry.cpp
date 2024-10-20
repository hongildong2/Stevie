#include "pch.h"
#include "D3D11MeshGeometry.h"
#include "D3D11Renderer.h"
#include "D3DUtil.h"
#include "D3D11DeviceResources.h"

using namespace DirectX::SimpleMath;

D3D11MeshGeometry::D3D11MeshGeometry(const EPrimitiveTopologyType type)
	: RMeshGeometry(type)
	, m_pRenderer(nullptr)
{
}

void D3D11MeshGeometry::Initialize(const D3D11Renderer* pRenderer, const void* pInVertexList, const UINT vertexSize, const UINT vertexCount, const void* pInIndexList, const UINT indexSize, const UINT indexCount)
{
	if (pRenderer == nullptr)
	{
		return;
		// assert;
	}

	m_pRenderer = pRenderer;

	auto* pDevice = m_pRenderer->GetDeviceResources()->GetD3DDevice();
	m_vertexStride = vertexSize;
	m_vertexOffset = 0;
	m_indexCount = indexCount;
	// TODO :: assert indexsize 32 or 16, 4 byte -> 32, or 16
	m_indexFormat = indexSize == 4 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;

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

void D3D11MeshGeometry::Draw() const
{
	// assert
	auto* pContext = m_pRenderer->GetDeviceResources()->GetD3DDeviceContext();
	ID3D11Buffer* vBuffers[1] = { m_vertexBuffer.Get() };

	pContext->IASetPrimitiveTopology(DX::D3D11::GetD3D11TopologyType(m_topologyType));
	pContext->IASetVertexBuffers(0, 1, vBuffers, &m_vertexStride, &m_vertexOffset);
	pContext->IASetIndexBuffer(m_indexBuffer.Get(), m_indexFormat, 0);

	pContext->DrawIndexed(m_indexCount, 0, 0);
}
