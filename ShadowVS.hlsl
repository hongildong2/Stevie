#include "RenderingCommons.hlsli"
#include "CoordTransform.hlsli"

cbuffer temppproro : register(b3)
{
	matrix lightViewProj;
};

// 여기서 light버퍼 인덱스 바꾸면서 참조하는거 포기, 정적인값으론 참조가되는데 동적인값으론 안됨 실행순서가 정의되지 않은듯 그냥 상수버퍼로 viewProj보내는게 나을듯
// 그냥 버텍스마다 실행하는거 말고 스트리밍? 그걸로 라이트 인덱스랑 넣어주는게 날듯
// TODO :: depth only pass를 Output Stream을 이용해 메쉬정보만 먼저 보내고 view proj만 바꾸면서 여러번 실행하기
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