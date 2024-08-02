#include "RenderingCommons.hlsli"
#include "CoordTransform.hlsli"



// ���⼭ light���� �ε��� �ٲٸ鼭 �����ϴ°� ����, �����ΰ����� �������Ǵµ� �����ΰ����� �ȵ� ��������� ���ǵ��� ������ �׳� ������۷� viewProj�����°� ������
// �׳� ���ؽ����� �����ϴ°� ���� ��Ʈ����? �װɷ� ����Ʈ �ε����� �־��ִ°� ����
// TODO :: depth only pass�� Output Stream�� �̿��� �޽������� ���� ������ view proj�� �ٲٸ鼭 ������ �����ϱ� :: https://learn.microsoft.com/en-us/windows/win32/direct3d11/d3d10-graphics-programming-guide-output-stream-stage
// �Ǵ� Dynamic Linking���� �ذ��غ��� :: https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/overviews-direct3d-11-hlsl-dynamic-linking
// ��Ե� ���Ľ���� ��ǲ�� ��Ȯ�Ͽ� �����ΰ� ������ �ʵ��� �ؾ��Ѵ�!!

float4 main(SamplingVertexShaderInput input) : SV_Position
{
	SamplingPixelShaderInput output;

	output.positionModel = input.positionModel;
	output.texcoord = input.texcoord;
	float4 pos = mul(float4(input.positionModel, 1.f), world);
	
	// float4 target = float4(shadowLightTest.positionWorld + shadowLightTest.direction, 1);
	pos = mul(pos, globalLights[1].view);
	output.positionProj = mul(pos, globalLights[1].proj);


	return output.positionProj;
}