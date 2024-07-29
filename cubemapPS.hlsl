#include "RenderingCommons.hlsli"


float4 main(PixelShaderInput input) : SV_TARGET
{
    // ����: �ؽ��� ��ǥ�� float3 �Դϴ�.
	float3 color = cubeMap.Sample(linearWrap, input.positionModel);

	return float4(color, 1);
}