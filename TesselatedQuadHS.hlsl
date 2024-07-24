#include "Common.hlsli"

#define NUM_CONTROL_POINTS 4

struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[4] : SV_TessFactor;
	float InsideTessFactor[2] : SV_InsideTessFactor;
};


HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<PixelShaderInput, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT Output;
	
	
	for (uint i = 0; i < NUM_CONTROL_POINTS; ++i)
	{
		float3 worldPos = ip[i].positionWorld;
		// TODO : APPLY LOD
		Output.EdgeTessFactor[i] = 64;
	}

	Output.InsideTessFactor[0] = 64;
	Output.InsideTessFactor[1] = 64;

	return Output;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("CalcHSPatchConstants")]
[maxtessfactor(64.0f)]
PixelShaderInput main(
	InputPatch<PixelShaderInput, NUM_CONTROL_POINTS> ip,
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID)
{
	return ip[i]; // no need to manipulate
}
