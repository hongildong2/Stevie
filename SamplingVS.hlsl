#include "RenderingCommons.hlsli"
#include "CoordTransform.hlsli"



SamplingPixelShaderInput main(VertexShaderInput input)
{
	SamplingPixelShaderInput output;

	output.positionModel = input.positionModel;
	output.texcoord = input.texcoordinate;
	float4 pos = mul(float4(input.positionModel, 1.f), world);
	
	float4 target = float4(globalLights[1].positionWorld + globalLights[1].direction, 1);
	
	//섀도우맵은 이친구로 변환을... selectedLight[0].viewProj;
	#ifdef SHADOW_MAP
	// 이래도 뎁스버퍼가 차질 않는다. 애초에 뎁스값이 넘 이상함.. not working
	// DSV는 문제 없다. CPU에 있는 값도 정상이다.
		float3 test = selectedLight[0].positionWorld + selectedLight[0].direction * 0.1;
	output.positionProj = mul(target, globalLights[1].viewProj);
	#else
	float4 viewPos = mul(pos, view);
	output.positionProj = mul(viewPos, proj);
	#endif

	return output;
}