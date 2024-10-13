#pragma once
#include "pch.h"
#include "IRenderer.h"
#include "IRenderResource.h"

enum class EBasicGeometry
{
	QUAD,
	BOX,
	SPHERE
};

enum class EPrimitiveTopologyType
{
	TRIANGLE,
	QUAD_PATCH,
	POINT,
	LINE
};


enum class EShaderType
{
	VERTEX_SHADER,
	PIXEL_SHADER,
	COMPUTE_SHADER,
	DOMAIN_SHADER,
	HULL_SHADER,
	GEOMETRY_SHADER
};

inline const WCHAR* ToString(const EShaderType type)
{
	switch (type)
	{
	case EShaderType::VERTEX_SHADER:
		return L"VS";
	case EShaderType::PIXEL_SHADER:
		return L"PS";
	case EShaderType::COMPUTE_SHADER:
		return L"CS";
	case EShaderType::DOMAIN_SHADER:
		return L"DS";
	case EShaderType::HULL_SHADER:
		return L"HS";
	case EShaderType::GEOMETRY_SHADER:
		return L"GS";
	default:
		// TODO :: assert
		return L"";
		break;
	}
}


struct Vertex
{
	DirectX::XMFLOAT3 positionModel;
	DirectX::XMFLOAT2 texcoordinate;
	DirectX::XMFLOAT3 normalModel;
	DirectX::XMFLOAT3 tangentModel;
};


struct MeshData
{
	std::vector<Vertex> verticies;
	std::vector<UINT> indicies;
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