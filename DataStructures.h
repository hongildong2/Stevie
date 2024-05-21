#pragma once

#include <d3d11.h>
#include "SimpleMath.h"
#include <string>
#include <vector>

// 몰랐어요
// Remember you should avoid putting using namespace statements at global scope in header files.C++ best practice is to use full name resolution in headers for types(i.e.DirectX::SimpleMath::Matrix m_proj;).
// header엔 함부로 작업하지 말자


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

struct VSConstantBuffer
{

};

// Temp,r

MeshData MakeSphere(const float radius, const int numSlices, const int numStacks, const DirectX::SimpleMath::Vector2 textureScale);