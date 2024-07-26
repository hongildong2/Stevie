#include "Common.hlsli"

#define NUM_CONTROL_POINTS 4

struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[4] : SV_TessFactor;
	float InsideTessFactor[2] : SV_InsideTessFactor;
};

cbuffer PSConstant : register(b0)
{
	float3 eyeWorld;
	float dummy;
	float metallicFactor;
	float aoFactor;
	float roughnessFactor;
	float t1;
};

HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<PixelShaderInput, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT Output;
	
	const float MAX_TESS = 64;
	
	float3 patchCenterWorldPos = 0;
	[unroll(NUM_CONTROL_POINTS)]
	for (uint i = 0; i < NUM_CONTROL_POINTS; ++i)
	{
		patchCenterWorldPos += ip[i].positionWorld;
	}
	
	patchCenterWorldPos /= float(NUM_CONTROL_POINTS);

	
	float dist = distance(patchCenterWorldPos, eyeWorld);
	
	dist = dist > 36 ? dist : 1;
	
	
	
	float lodTessFactor = MAX_TESS / dist;
	
	Output.EdgeTessFactor[0] = lodTessFactor;
	Output.EdgeTessFactor[1] = lodTessFactor;
	Output.EdgeTessFactor[2] = lodTessFactor;
	Output.EdgeTessFactor[3] = lodTessFactor;
	
	Output.InsideTessFactor[0] = lodTessFactor;
	Output.InsideTessFactor[1] = lodTessFactor;

	return Output;
}

[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("CalcHSPatchConstants")]
[maxtessfactor(64.0f)]
	PixelShaderInput
	main(

	InputPatch<PixelShaderInput, NUM_CONTROL_POINTS> ip,
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID)
{
	return ip[i]; // no need to manipulate
}
