#pragma once

#include "SimpleMath.h"
#include "SceneLights.h"

struct MeshConstants
{
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix worldIT;
	DirectX::SimpleMath::Matrix worldInv;

	BOOL bUseHeightMap;
	float heightScale;
	UINT meshLightsCount;
	float mcDummy;
};
static_assert(sizeof(MeshConstants) % 16 == 0, "Constant Buffer Alignment");

struct TextureFiles
{
	const wchar_t* albedoName;
	const wchar_t* aoName;
	const wchar_t* heightName;
	const wchar_t* metallicName;
	const wchar_t* normalName;
	const wchar_t* roughnessName;
};

constexpr TextureFiles NO_MESH_TEXTURE =
{
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
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
	float IBLStrength;
};

constexpr Material DEFAULT_MATERIAL =
{
	0.7f,
	0.3f,
	1.f,
	1.f,

	TRUE,
	DirectX::SimpleMath::Vector3(1.f, 1.f, 1.f),

	0.8f,
	0.2f,
	0.5f,
	1.f
};

static_assert(sizeof(Material) % 16 == 0, "Material Alignment");


struct Vertex
{
	DirectX::SimpleMath::Vector3 positionModel;
	DirectX::SimpleMath::Vector2 texcoordinate;
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
