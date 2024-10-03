#pragma once
#include "pch.h"



class MeshGeometry
{
public:
private:
	const UINT m_vertexStride;
	const UINT m_vertexOffset;
	const UINT m_indexCount;

	const DXGI_FORMAT m_indexFormat;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

};

