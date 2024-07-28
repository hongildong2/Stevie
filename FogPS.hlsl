#include "ScreenSpace.hlsli"
#include "RenderingCommons.hlsli"

Texture2D<float3> RenderResult : register(t100);
Texture2D<float> depthOnlyTex : register(t101);

float4 main(SamplingPixelShaderInput input) : SV_TARGET
{
	float4 viewSpacePosition = TexcoordToView(input.texcoord, depthOnlyTex, linearClamp, invProj);
	
	float dist = length(viewSpacePosition.xyz); // ÇöÀç ÁÂÇ¥°è´Â ºä °ø°£

	
	float3 fogColor = float3(1, 1, 1); // TODO : Sample noise voxel texture, dynamic fog
	
	float fogMin = 1.0;
	float fogMax = 10.0;
	
	float distFog = saturate((dist - fogMin) / (fogMax - fogMin));
	
	float fogStrength = 1.0;
	float fogFactor = exp(-distFog * fogStrength);

	
	float3 color = RenderResult.Sample(linearClamp, input.texcoord).rgb;
	
	color = lerp(fogColor, color, fogFactor);
	
	return float4(color, 1.0);
}