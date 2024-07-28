#include "RenderingCommons.hlsli"


float4 main(PixelShaderInput input) : SV_TARGET
{
    // 주의: 텍스춰 좌표가 float3 입니다.
	return cubeMap.Sample(linearWrap, input.positionWorld.xyz);
}