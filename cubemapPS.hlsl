#include "RenderingCommons.hlsli"


float4 main(PixelShaderInput input) : SV_TARGET
{
    // ����: �ؽ��� ��ǥ�� float3 �Դϴ�.
	return cubeMap.Sample(linearWrap, input.positionWorld.xyz);
}