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
		MY_ASSERT(FALSE);
		break;
	}
}


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
	std::vector<UINT> indicies;
};


struct GlobalConstant
{
	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix proj;
	DirectX::SimpleMath::Matrix viewProj;

	DirectX::SimpleMath::Matrix invView;
	DirectX::SimpleMath::Matrix invProj;
	DirectX::SimpleMath::Matrix invViewProj;

	DirectX::SimpleMath::Vector3 eyeWorld;
	float globalTime;

	DirectX::SimpleMath::Vector3 eyeDir;
	UINT globalLightsCount;

	float nearZ;
	float farZ;
	DirectX::SimpleMath::Vector2 gcDummy;
};

static_assert(sizeof(GlobalConstant) % 16 == 0, "Constant Buffer Alignment");

struct MeshConstant
{
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix worldIT;
	DirectX::SimpleMath::Matrix worldInv;
};
static_assert(sizeof(MeshConstant) % 16 == 0, "Constant Buffer Alignment");