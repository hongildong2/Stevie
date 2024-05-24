#pragma once
#include "GraphicsCommon.h"


#include <d3d11.h>
#include "SimpleMath.h"
/*
* reference : https://github.com/microsoft/DirectXTK/wiki/ModelMeshPart
*
*/



struct Vertex
{
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector2 textureCoordination;
	DirectX::SimpleMath::Vector3 normalModel;
	DirectX::SimpleMath::Vector3 tangentModel;
};


struct MeshData
{
	std::vector<Vertex> verticies;
	std::vector<uint32_t> indicies;
};

namespace GeometryGenerator
{
	/*
	* reference : honglab introduction to graphics course
	*/
	MeshData MakeSphere(const float radius, const unsigned int numSlices, const unsigned int numStacks);
	MeshData MakeBox(const float scale);
	MeshData MakeSquare();
}


class ModelMeshPart
{
public:
	ModelMeshPart(MeshData& mesh, ID3D11Device1* device);
	ModelMeshPart(const ModelMeshPart& other);

	void Draw(ID3D11DeviceContext1* context) const;

private:
	UINT m_vertexStride;
	UINT m_vertexOffset;
	UINT m_indexCount;
	D3D_PRIMITIVE_TOPOLOGY m_primitiveTopology;
	DXGI_FORMAT m_indexFormat;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
};

