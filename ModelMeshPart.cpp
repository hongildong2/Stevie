#include "pch.h"
#include "ModelMeshPart.h"

// 중간에 메쉬를 만드는 경우는 없으므로 그냥 생성자 이용
ModelMeshPart::ModelMeshPart(MeshData& mesh, ID3D11Device1* device)
{
	m_vertexStride = sizeof(Vertex);
	m_vertexOffset = 0;

	m_indexFormat = DXGI_FORMAT_R32_UINT;
	m_indexCount = static_cast<UINT>(mesh.indicies.size());
	// init vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.Usage = D3D11_USAGE_DEFAULT; // read only?
	bufferDesc.ByteWidth = sizeof(Vertex) * static_cast<UINT>(mesh.verticies.size());
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0; // no access
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = mesh.verticies.data(); // real data!
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0; // 사이즈 보정용?

	HRESULT hr = device->CreateBuffer(&bufferDesc, &InitData, &m_vertexBuffer);
	DX::ThrowIfFailed(hr);

	// index buffer
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(uint32_t) * static_cast<UINT>(mesh.indicies.size());
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = mesh.indicies.data();
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0; // 사이즈 보정용?

	hr = device->CreateBuffer(&bufferDesc, &InitData, &m_indexBuffer);
	DX::ThrowIfFailed(hr);
}

void ModelMeshPart::Draw(ID3D11DeviceContext1* context) const
{
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &m_vertexStride, &m_vertexOffset);
	context->IASetIndexBuffer(m_indexBuffer.Get(), m_indexFormat, 0);

	context->DrawIndexed(m_indexCount, 0, 0);
}
