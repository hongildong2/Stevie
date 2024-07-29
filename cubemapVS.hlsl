#include "RenderingCommons.hlsli"

PixelShaderInput main(VertexShaderInput input)
{
    // ���ʿ��� ����鵵 VertexShaderInput�� ���Ͻ��ױ� ������ ä����� �մϴ�.
    
	PixelShaderInput output;
	float4 pos = float4(input.positionModel, 1.0f);

	pos = mul(pos, world); // Identity

	output.positionWorld = pos.xyz;
	output.tangentWorld = mul(float4(input.tangentModel, 0.f), worldIT);
    
	float4 normal = float4(input.normalModel, 0.0f);
	output.normalWorld = mul(normal, worldIT).xyz;
	output.normalWorld = normalize(output.normalWorld);

	pos = mul(pos, view);
    
	pos = mul(pos, proj);
	output.positionProjection = pos;

	output.texcoordinate = input.texcoordinate;
	output.positionModel = input.positionModel;

	return output;
}
