#pragma once
#include "pch.h"
#include "../RMeshGeometry.h"


class D3D11Renderer;

class D3D11MeshGeometry final : public RMeshGeometry
{
public:
	D3D11MeshGeometry(const EPrimitiveTopologyType type, const EMeshType meshType);
	~D3D11MeshGeometry() = default;

	void Initialize(const D3D11Renderer* pRenderer, const void* pInVertexList, const UINT vertexSize, const UINT vertexCount, const void* pInIndexList, const UINT indexSize, const UINT indexCount);

	inline ID3D11Buffer* GetVertexBuffer() const
	{
		return m_vertexBuffer.Get();
	}

	inline ID3D11Buffer* GetIndexBuffer() const
	{
		return m_indexBuffer.Get();
	}

private:
	const D3D11Renderer* m_pRenderer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
};

