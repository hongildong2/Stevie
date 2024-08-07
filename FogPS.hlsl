#include "ScreenSpace.hlsli"
#include "RenderingCommons.hlsli"

Texture2D<float3> RenderResult : register(t100);
Texture2D<float> depthOnlyTex : register(t101);

float4 main(SamplingPixelShaderInput input) : SV_TARGET
{
	float4 positionViewSpace = TexcoordToView(input.texcoord, depthOnlyTex, linearClamp, invProj);
	float dist = length(positionViewSpace.xyz);
	float3 color = RenderResult.Sample(linearClamp, input.texcoord).rgb;
	
	// fog
	float3 fogColor = float3(1, 1, 1);
	
	float fogMin = 5.0;
	float fogMax = 40.0;
	
	float distFog = saturate((dist - fogMin) / (fogMax - fogMin));
	
	float fogStrength = 1.1;
	float fogFactor = exp(-distFog * fogStrength);

	
	
	color = lerp(fogColor, color, fogFactor);
	
	return float4(color, 1.0);
}