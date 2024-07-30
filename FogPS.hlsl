#include "CoordTransform.hlsli"
#include "RenderingCommons.hlsli"

Texture2D<float3> RenderResult : register(t100);
Texture2D<float> depthOnlyTex : register(t101);

float4 main(SamplingPixelShaderInput input) : SV_TARGET
{
	float4 viewSpacePosition = TexcoordToView(input.texcoord, depthOnlyTex, linearClamp, invProj);
	
	// �ٴٴ� fog ����
	const float4 REFERENCE_OCEAN_PLANE = float4(0, 0.4, 0, 1);
	// ���̴��� ������ Column-major matrix�� �ǵ��� �������Ƿ�, ���ʹ� row vector�� �Ǿ���Ѵ�
	const float4 REFERENCE_HEIGHT_IN_VIEW = mul(REFERENCE_OCEAN_PLANE, view);
	
	// const float4 REFERENCE_HEIGHT_IN_VIEW = mul(view, REFERENCE_OCEAN_PLANE); view�� Row major matrix ���, ���ʹ� column vector ���
	float dist = min(length(REFERENCE_HEIGHT_IN_VIEW), length(viewSpacePosition.xyz)); // ���� ��ǥ��� �� ����

	
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