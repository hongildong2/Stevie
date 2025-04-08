#pragma once

class D3D11MeshGeometry
{
public:
	D3D11MeshGeometry() = default;
	~D3D11MeshGeometry() = default;

	inline ID3D11Buffer* GetVertexBuffer() const
	{
		return m_vertexBuffer.Get();
	}

	inline ID3D11Buffer* GetIndexBuffer() const
	{
		return m_indexBuffer.Get();
	}

protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
};

