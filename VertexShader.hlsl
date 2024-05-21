struct VertexShaderInput
{
	float3 positionModel : POSITION; //�� ��ǥ���� ��ġ position
	float3 normalModel : NORMAL; // �� ��ǥ���� normal    
	float2 texcoordinate : TEXCOORD;
	float3 tangentModel : TANGENT;
};

struct PixelShaderInput
{
	float4 positionProjection : SV_POSITION; // Screen position
	float3 positionWorld : POSITION0; // World position (���� ��꿡 ���)
	float3 normalWorld : NORMAL0;
	float2 texcoordinate : TEXCOORD0;
	float3 tangentWorld : TANGENT0;
	float3 positionModel : POSITION1; // Volume casting ������
};

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	
	output.normalWorld = input.normalModel;
	output.positionWorld = input.positionModel;
	output.positionProjection = float4(input.positionModel.xyz, 0);
	output.texcoordinate = input.texcoordinate;
	output.tangentWorld = input.tangentModel;
	output.positionModel = input.positionModel;
	
	
	return output;
}