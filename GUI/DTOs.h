#pragma once
#include "pch.h"
#include "ModelCommon.h"
#include "OceanConstants.h"

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
	ocean::RenderingParameters renderingParams;
	bool bResetInitialSpectrumData;
};

struct PostProcessDTO
{
	float bloomStrength;
	float exposure;
	float gamma;
};