#pragma once
#include "pch.h"
#include "ModelCommon.h"
#include "OceanConstants.h"

struct PosWorldDTO
{
	DirectX::SimpleMath::Vector3 pos;
};

struct RotationWorldDTO
{
	float yaw;
	float pitch;
};

struct MaterialDTO : public Material
{
};

struct LightDTO : public LightData
{

};

struct OceanDTO
{
	ocean::InitialSpectrumWaveConstant initialWaveConstants;
	std::array<ocean::InitialSpectrumParameter, ocean::CASCADE_COUNT> initialSpectrumParameters;
	std::array<ocean::CombineParameter, ocean::CASCADE_COUNT> cascadeCombineParameters;
};

struct PostProcessDTO
{
	float bloomStrength;
	float exposure;
	float gamma;
};