#include "RenderingCommons.hlsli"
#include "CoordTransform.hlsli"



SamplingPixelShaderInput main(VertexShaderInput input)
{
	SamplingPixelShaderInput output;

	output.positionModel = input.positionModel;
	output.texcoord = input.texcoordinate;
	float4 pos = mul(float4(input.positionModel, 1.f), world);
	
	float4 target = float4(globalLights[1].positionWorld + globalLights[1].direction, 1);
	
	//��������� ��ģ���� ��ȯ��... selectedLight[0].viewProj;
	#ifdef SHADOW_MAP
	// �̷��� �������۰� ���� �ʴ´�. ���ʿ� �������� �� �̻���.. not working
	// DSV�� ���� ����. CPU�� �ִ� ���� �����̴�.
		float3 test = selectedLight[0].positionWorld + selectedLight[0].direction * 0.1;
	output.positionProj = mul(target, globalLights[1].viewProj);
	#else
	float4 viewPos = mul(pos, view);
	output.positionProj = mul(viewPos, proj);
	#endif

	return output;
}