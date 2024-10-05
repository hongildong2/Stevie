#pragma once
#include "pch.h"

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


typedef enum ESAMPLER_FILTER
{
	FILTER_MIN_MAG_MIP_POINT = 0,
	FILTER_MIN_MAG_POINT_MIP_LINEAR = 0x1,
	FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x4,
	FILTER_MIN_POINT_MAG_MIP_LINEAR = 0x5,
	FILTER_MIN_LINEAR_MAG_MIP_POINT = 0x10,
	FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x11,
	FILTER_MIN_MAG_LINEAR_MIP_POINT = 0x14,
	FILTER_MIN_MAG_MIP_LINEAR = 0x15,
	FILTER_ANISOTROPIC = 0x55,
	FILTER_COMPARISON_MIN_MAG_MIP_POINT = 0x80,
	FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR = 0x81,
	FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x84,
	FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR = 0x85,
	FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT = 0x90,
	FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x91,
	FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT = 0x94,
	FILTER_COMPARISON_MIN_MAG_MIP_LINEAR = 0x95,
	FILTER_COMPARISON_ANISOTROPIC = 0xd5,
	FILTER_MINIMUM_MIN_MAG_MIP_POINT = 0x100,
	FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR = 0x101,
	FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x104,
	FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR = 0x105,
	FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT = 0x110,
	FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x111,
	FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT = 0x114,
	FILTER_MINIMUM_MIN_MAG_MIP_LINEAR = 0x115,
	FILTER_MINIMUM_ANISOTROPIC = 0x155,
	FILTER_MAXIMUM_MIN_MAG_MIP_POINT = 0x180,
	FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR = 0x181,
	FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x184,
	FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR = 0x185,
	FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT = 0x190,
	FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x191,
	FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT = 0x194,
	FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR = 0x195,
	FILTER_MAXIMUM_ANISOTROPIC = 0x1d5
};

typedef enum ESAMPLER_TEXTURE_ADDRESS_MODE
{
	TEXTURE_ADDRESS_WRAP = 1,
	TEXTURE_ADDRESS_MIRROR = 2,
	TEXTURE_ADDRESS_CLAMP = 3,
	TEXTURE_ADDRESS_BORDER = 4,
	TEXTURE_ADDRESS_MIRROR_ONCE = 5
};

typedef enum ESAMPLER_COMPARISON_FUNC
{
	COMPARISON_NEVER = 1,
	COMPARISON_LESS = 2,
	COMPARISON_EQUAL = 3,
	COMPARISON_LESS_EQUAL = 4,
	COMPARISON_GREATER = 5,
	COMPARISON_NOT_EQUAL = 6,
	COMPARISON_GREATER_EQUAL = 7,
	COMPARISON_ALWAYS = 8
};

typedef struct SAMPLER_DESC {
	ESAMPLER_FILTER               Filter;
	ESAMPLER_TEXTURE_ADDRESS_MODE AddressU;
	ESAMPLER_TEXTURE_ADDRESS_MODE AddressV;
	ESAMPLER_TEXTURE_ADDRESS_MODE AddressW;
	FLOAT                      MipLODBias;
	UINT                       MaxAnisotropy;
	ESAMPLER_COMPARISON_FUNC      ComparisonFunc;
	FLOAT                      BorderColor[4];
	FLOAT                      MinLOD;
	FLOAT                      MaxLOD;
} SAMPLER_DESC;