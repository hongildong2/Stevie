#pragma once
#include "pch.h"
#include "RenderConfig.h"


enum class EBasicMeshGeometry
{
	QUAD,
	CUBE,
	SPHERE,
	TESSELLATED_QUAD
};

enum class EPrimitiveTopologyType
{
	TRIANGLE_LIST,
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
		return L"";
		break;
	}
}

struct RVertex
{
	DirectX::SimpleMath::Vector3 positionModel;
	DirectX::SimpleMath::Vector2 texcoordinate;
	DirectX::SimpleMath::Vector3 normalModel;
	DirectX::SimpleMath::Vector3 tangentModel;
};


struct MeshData
{
	std::vector<RVertex> verticies;
	std::vector<UINT> indicies;
};

struct RGlobalConstant
{
	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix proj;
	DirectX::SimpleMath::Matrix viewProj;

	DirectX::SimpleMath::Matrix invView;
	DirectX::SimpleMath::Matrix invProj;
	DirectX::SimpleMath::Matrix invViewProj;

	DirectX::SimpleMath::Vector3 eyeWorld;
	FLOAT globalTime;

	DirectX::SimpleMath::Vector3 eyeDir;
	UINT globalLightsCount;

	FLOAT nearZ;
	FLOAT farZ;
	UINT shadowingLightsCount;
	FLOAT dummy;
};

static_assert(sizeof(RGlobalConstant) % 16 == 0, "Constant Buffer Alignment");

struct RMeshConstant
{
	RMeshConstant(DirectX::SimpleMath::Matrix& worldRowMat, BOOL bUseHeightMap)
		: bUseHeightMap(bUseHeightMap)
	{
		world = worldRowMat.Transpose();
		worldInv = world.Invert();
		worldIT = worldInv.Transpose();
	}

	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix worldIT;
	DirectX::SimpleMath::Matrix worldInv;

	BOOL bUseHeightMap;
	DirectX::SimpleMath::Vector3 DUMMY;
};
static_assert(sizeof(RMeshConstant) % 16 == 0, "Constant Buffer Alignment");

enum class ELightType
{
	DIRECTIONAL,
	SPOT
};

struct RLightConstant
{
	RLightConstant() = default;
	RLightConstant(ELightType lightType, DirectX::SimpleMath::Vector3 dirWorld, DirectX::SimpleMath::Vector3 posWorld, DirectX::SimpleMath::Vector3 color, float radiance, float fallOffStart, float fallOffEnd, float spotPower, float radius)
		: type(static_cast<UINT>(lightType))
		, direction(dirWorld)
		, positionWorld(posWorld)
		, color(color)
		, radiance(radiance)
		, fallOffStart(fallOffStart)
		, fallOffEnd(fallOffEnd)
		, radius(radius)
		, spotPower(spotPower)
		, dummy(0.f)
	{
		switch (lightType)
		{
		case ELightType::DIRECTIONAL:
			proj = renderConfig::GetDirectionalLightProjRowMat();
			break;
		case ELightType::SPOT:
			proj = renderConfig::GetSpotLightProjRowMat();
			break;
		default:
			MY_ASSERT(false);
		}

		DirectX::SimpleMath::Vector3 vLookAt(posWorld + direction);
		vLookAt.Normalize();

		view = DirectX::SimpleMath::Matrix::CreateLookAt(posWorld, vLookAt, DirectX::SimpleMath::Vector3(0.f, 1.f, 0.f));

		proj = proj.Transpose();
		invProj = proj.Invert();
		view = view.Transpose();
	}

	DirectX::SimpleMath::Vector3 direction;
	FLOAT radiance;

	DirectX::SimpleMath::Vector3 color;
	FLOAT fallOffStart;

	DirectX::SimpleMath::Vector3 positionWorld;
	FLOAT fallOffEnd;

	FLOAT spotPower;
	UINT type;
	FLOAT radius;
	FLOAT dummy;

	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix proj;
	DirectX::SimpleMath::Matrix invProj;
};
static_assert(sizeof(RLightConstant) % 16 == 0, "CONSTANT BUFFER ALIGNMENT");