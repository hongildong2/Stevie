#include "pch.h"
#include "DataStructures.h"

MeshData MakeSphere(const float radius, const int numSlices, const int numStacks, const DirectX::SimpleMath::Vector2 textureScale)
{
	const float dTheta = -DirectX::XM_2PI / float(numSlices);
	const float dPhi = -DirectX::XM_PI / float(numStacks);

	MeshData meshData;
	meshData.verticies.reserve(numSlices * numStacks + 1);

	std::vector<Vertex>& verticies = meshData.verticies;

	// creating verticies per slice rotating 
	for (int j = 0; j <= numStacks; j++)
	{
		DirectX::SimpleMath::Vector3 stackStartPoint = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.f, -radius, 0.f), DirectX::SimpleMath::Matrix::CreateRotationZ(dPhi * j));

		for (int i = 0; i <= numSlices; i++)
		{
			Vertex v;

			v.position = DirectX::SimpleMath::Vector3::Transform(stackStartPoint, DirectX::SimpleMath::Matrix::CreateRotationY(dTheta * float(i)));

			v.normalModel = v.position;
			v.normalModel.Normalize();
			v.textureCoordination = DirectX::SimpleMath::Vector2(float(i) / numSlices, 1.f - float(j) / numStacks) * textureScale;

			DirectX::SimpleMath::Vector3 biTangent = DirectX::SimpleMath::Vector3(0.f, 1.f, 0.f);

			DirectX::SimpleMath::Vector3 normalOrth = v.normalModel - biTangent.Dot(v.normalModel) * v.normalModel;
			normalOrth.Normalize();

			verticies.push_back(v);
		}
	}

	std::vector<uint32_t>& indices = meshData.indicies;

	for (int j = 0; j < numStacks; ++j)
	{
		const int offset = (numSlices + 1) * j;
		for (int i = 0; i < numSlices; ++i)
		{
			indices.push_back(offset + i);
			indices.push_back(offset + i + numSlices + 1);
			indices.push_back(offset + i + 1 + numSlices + 1);

			indices.push_back(offset + i);
			indices.push_back(offset + i + 1 + numSlices + 1);
			indices.push_back(offset + i + 1);
		}
	}

	return meshData;
}
