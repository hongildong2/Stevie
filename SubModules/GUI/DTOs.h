#pragma once
#include "pch.h"

#include "SubModules\Render\Mesh\MeshCommon.h"
#include "Core\Ocean\OceanConstants.h"
#include "Core\Light.h"

struct PosWorldDTO
{
	DirectX::SimpleMath::Vector4 pos;
};

struct RotationWorldDTO
{
	float yaw;
	float pitch;
};

struct MaterialDTO : public Material
{
	MaterialDTO(const Material& rhs);
};

struct LightDTO : public LightData
{
	LightDTO(const LightData& rhs);
};

struct OceanDTO
{
	ocean::OceanConfigurationConstant oceanConfiguration;
	std::array<ocean::InitialSpectrumParameter, ocean::CASCADE_COUNT> initialSpectrumParameters;
	std::array<ocean::CombineParameter, ocean::CASCADE_COUNT> cascadeCombineParameters;
	ocean::RenderingParameter renderingParams;
	bool bResetInitialSpectrumData;
};

struct PostProcessDTO
{
	float bloomStrength;
	float exposure;
	float gamma;
	float fogStrength;
};

struct MeshConstDTO
{
	BOOL bUseHeightMap;
	float heightScale;
};