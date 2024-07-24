#include "Common.hlsli"

struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[4] : SV_TessFactor;
	float InsideTessFactor[2] : SV_InsideTessFactor;
};

Texture2D displacementMap : register(t0);

Texture2D normalMap : register(t1);

SamplerState linearClamp : register(s0);

#define NUM_CONTROL_POINTS 4

[domain("quad")]
PixelShaderInput main(
	HS_CONSTANT_DATA_OUTPUT input,
	float2 domain : SV_DomainLocation, // 어떤 좌표계로들어온다는거야 ?ㅅㅂ
	const OutputPatch<PixelShaderInput, NUM_CONTROL_POINTS> patch)
{
	PixelShaderInput Output;
	
	PixelShaderInput topLeft = patch[0]; // has tex coord (0,0)
	PixelShaderInput bottomLeft = patch[1];
	PixelShaderInput bottomRight = patch[2]; // has tex coord (1,1)
	PixelShaderInput topRight = patch[3]; 

	Output.texcoordinate = domain; // 이렇게 되는건가??
	
	Output.positionModel.y = 0;
	Output.positionModel.x = lerp(topLeft.positionModel.x, topRight.positionModel.x, domain.x);
	Output.positionModel.y = lerp(topLeft.positionModel.y, topRight.positionModel.y, domain.y);
	
	
	// normal model은 텍스쳐 매핑
	// 모델좌표계에 displacement맵을 해야하는가? 월드좌표계에?
	// 일단 모델좌표계에 displacement 맵 적용, 파라미터 붙여서 조절가능하게 상수버퍼
	
	// world좌표는 변환행렬 붙여서 다시
	// projection도 다시
	// normal world도 다시 -> tangent 다시
	
	
	return Output;
}
