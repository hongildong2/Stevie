#pragma once

#include "pch.h"
#include "SimpleMath.h"

struct VSConstants
{
	DirectX::SimpleMath::Matrix worldMatrix;
	DirectX::SimpleMath::Matrix worldMatrixIT;
	DirectX::SimpleMath::Matrix viewMatrix;
	DirectX::SimpleMath::Matrix projMatrix;
};
static_assert(sizeof(VSConstants) % 16 == 0, "Constant Buffer Alignment");


struct TextureFiles
{
	const wchar_t* albedoName;
	const wchar_t* aoName;
	const wchar_t* heightName;
	const wchar_t* metallicName;
	const wchar_t* normalName;
	const wchar_t* roughnessName;
};

struct Material
{
	float metallicFactor;
	float aoFactor;
	float roughnessFactor;
	float t1;

	BOOL bUseTexture;
	DirectX::SimpleMath::Vector3 albedo;

	float metallic;
	float roughness;
	float specular; // default 0.5, water 0.255
	float dummy;
};

constexpr Material DEFAULT_MATERIAL =
{
	0.7f,
	0.3f,
	1.f,
	1.f,

	TRUE,
	DirectX::SimpleMath::Vector3(1.f, 1.f, 1.f),

	0.1f,
	0.1f,
	0.5f,
	0.f
};


struct PSConstants
{
	DirectX::SimpleMath::Vector3 eyeWorld;
	float dummy;
	Material material;
};

static_assert(sizeof(PSConstants) % 16 == 0, "PSConstants Alignment");


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
	MeshData MakeSquare(const float scale);
	void MakeCWQuadPatches(unsigned int splitLevel, MeshData* const outMesh);
}