#pragma once
#include "pch.h"

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

struct TextureFiles
{
	std::wstring albedoName;
	std::wstring aoName;
	std::wstring heightName;
	std::wstring metallicName;
	std::wstring normalName;
	std::wstring roughnessName;
	std::wstring emissiveName;
	std::wstring opacityName;
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

	BOOL bUseAlbedoTexture;
	BOOL bUseAOTexture;
	BOOL bUseHeightTexture;
	BOOL bUseMetallicTexture;

	BOOL bUseNormalTexture;
	BOOL bUseRoughnessTexture;
	BOOL bUseEmissiveTexture;
	BOOL bUseOpacityTexture;
};

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