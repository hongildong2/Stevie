#include "RenderingCommons.hlsli"

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;
	
	output.normalWorld = mul(float4(input.normalModel.xyz, 0.f), worldIT).xyz;
	output.normalWorld = normalize(output.normalWorld);
	output.positionWorld = mul(float4(input.positionModel.xyz, 1.f), world).xyz;
	

	if (bUseHeightMap)
	{
		float height = heightTex.SampleLevel(linearWrap, input.texcoordinate, 0).r;
		height = height * 2.0 - 1.0; // ���̷����ұ�? ������, �� ������ 0 ~ 1 �Ǵ� 0 ~ 255�ϱ�, -1 ~ 1 �� ��ǥ�躯ȯ
		input.positionModel = input.positionModel + heightScale * height * input.normalModel;
	}
	
	 
	output.positionProjection = mul(float4(output.positionWorld, 1.0), view);
	output.positionProjection = mul(output.positionProjection, proj);
	
	output.texcoordinate = input.texcoordinate;
	output.tangentWorld = mul(float4(input.tangentModel.xyz, 0.f), worldIT).xyz;
	output.positionModel = input.positionModel;
		
	return output;
}