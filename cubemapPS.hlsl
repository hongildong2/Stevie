#include "RenderingCommons.hlsli"


float4 main(PixelShaderInput input) : SV_TARGET
{
    // 주의: 텍스춰 좌표가 float3 입니다.
	float3 color = cubeMap.Sample(linearWrap, input.positionModel);

	return float4(color, 1);
}