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
	float2 domain : SV_DomainLocation, // � ��ǥ��ε��´ٴ°ž� ?����
	const OutputPatch<PixelShaderInput, NUM_CONTROL_POINTS> patch)
{
	PixelShaderInput Output;
	
	PixelShaderInput topLeft = patch[0]; // has tex coord (0,0)
	PixelShaderInput bottomLeft = patch[1];
	PixelShaderInput bottomRight = patch[2]; // has tex coord (1,1)
	PixelShaderInput topRight = patch[3]; 

	Output.texcoordinate = domain; // �̷��� �Ǵ°ǰ�??
	
	Output.positionModel.y = 0;
	Output.positionModel.x = lerp(topLeft.positionModel.x, topRight.positionModel.x, domain.x);
	Output.positionModel.y = lerp(topLeft.positionModel.y, topRight.positionModel.y, domain.y);
	
	
	// normal model�� �ؽ��� ����
	// ����ǥ�迡 displacement���� �ؾ��ϴ°�? ������ǥ�迡?
	// �ϴ� ����ǥ�迡 displacement �� ����, �Ķ���� �ٿ��� ���������ϰ� �������
	
	// world��ǥ�� ��ȯ��� �ٿ��� �ٽ�
	// projection�� �ٽ�
	// normal world�� �ٽ� -> tangent �ٽ�
	
	
	return Output;
}
