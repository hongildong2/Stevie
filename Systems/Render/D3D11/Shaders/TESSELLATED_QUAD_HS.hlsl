#include "INCL_ShaderTypes.hlsli"
#define NUM_CONTROL_POINTS 4

struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[4] : SV_TessFactor;
	float InsideTessFactor[2] : SV_InsideTessFactor;
};

cbuffer GlobalConstants : register(b0)
{
	GlobalConstant globalConstant;
}




HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(InputPatch<PixelShaderInput, NUM_CONTROL_POINTS> ip)
{
	HS_CONSTANT_DATA_OUTPUT Output;
	
	#ifdef DEPTH_ONLY
	float lodTessFactor = 2;
	#else
	const float MAX_TESS = 64;
	
	float3 patchCenterWorldPos = 0;
	[unroll(NUM_CONTROL_POINTS)]
	for (uint i = 0; i < NUM_CONTROL_POINTS; ++i)
	{
		patchCenterWorldPos += ip[i].positionWorld;
	}
	
	patchCenterWorldPos /= float(NUM_CONTROL_POINTS);

	
	float dist = distance(patchCenterWorldPos, globalConstant.eyeWorld);
	
	dist = dist > 36 ? dist : 1;
	
	
	
	float lodTessFactor = float(MAX_TESS) / dist * 3.0;

	#endif
	

		
	Output.EdgeTessFactor[0] = lodTessFactor;
	Output.EdgeTessFactor[1] = lodTessFactor;
	Output.EdgeTessFactor[2] = lodTessFactor;
	Output.EdgeTessFactor[3] = lodTessFactor;
	
	Output.InsideTessFactor[0] = lodTessFactor;
	Output.InsideTessFactor[1] = lodTessFactor;

	return Output;
}

[domain("quad")]
[partitioning("pow2")]
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
