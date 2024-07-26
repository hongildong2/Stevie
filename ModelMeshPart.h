#pragma once
#include "GraphicsCommon.h"

#include "BasicGeometry.h"
/*
* reference : https://github.com/microsoft/DirectXTK/wiki/ModelMeshPart
*
*/






class ModelMeshPart final
{
public:
	ModelMeshPart(MeshData& mesh, ID3D11Device1* device);
	ModelMeshPart(const ModelMeshPart& other) = delete;
	~ModelMeshPart() = default;

	void Draw(ID3D11DeviceContext1* context) const;

private:
	UINT m_vertexStride;
	UINT m_vertexOffset;
	UINT m_indexCount;

	DXGI_FORMAT m_indexFormat;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
};

