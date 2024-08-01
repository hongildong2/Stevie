#include "RenderingCommons.hlsli"
#include "CoordTransform.hlsli"

cbuffer temppproro : register(b3)
{
	matrix lightViewProj;
};

// ���⼭ light���� �ε��� �ٲٸ鼭 �����ϴ°� ����, �����ΰ����� �������Ǵµ� �����ΰ����� �ȵ� ��������� ���ǵ��� ������ �׳� ������۷� viewProj�����°� ������
// �׳� ���ؽ����� �����ϴ°� ���� ��Ʈ����? �װɷ� ����Ʈ �ε����� �־��ִ°� ����
// TODO :: depth only pass�� Output Stream�� �̿��� �޽������� ���� ������ view proj�� �ٲٸ鼭 ������ �����ϱ�
float4 main(SamplingVertexShaderInput input) : SV_Position
{
	SamplingPixelShaderInput output;

	output.positionModel = input.positionModel;
	output.texcoord = input.texcoord;
	float4 pos = mul(float4(input.positionModel, 1.f), world);
	
	// float4 target = float4(shadowLightTest.positionWorld + shadowLightTest.direction, 1);
	output.positionProj = mul(pos, globalLights[1].viewProj);


	return output.positionProj;
}