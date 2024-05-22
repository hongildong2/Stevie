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
	DirectX::SimpleMath::Vector3 normalModel;
	DirectX::SimpleMath::Vector2 textureCoordination;
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
	MeshData MakeSphere(const float radius, const unsigned int numSlices, const unsigned int numStacks, const DirectX::SimpleMath::Vector2 textureScale);
}


class ModelMeshPart
{
public:
	ModelMeshPart(MeshData& mesh, ID3D11Device1* device);
	ModelMeshPart(const ModelMeshPart& other);

	void Draw(ID3D11DeviceContext1* context) const;

private:
	UINT m_VertexStride;
	UINT m_VertexOffset;
	UINT m_IndexCount;
	D3D_PRIMITIVE_TOPOLOGY m_PrimitiveTopology;
	DXGI_FORMAT m_IndexFormat;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_IndexBuffer;
};

