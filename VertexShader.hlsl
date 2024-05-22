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

cbuffer transform : register(b0)
{
	float4x4 world;
	float4x4 worldIT;
	float4x4 view;
	float4x4 proj;
};

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	
	output.normalWorld = mul(float4(input.normalModel.xyz, 0.f), worldIT).xyz;
	output.positionWorld = mul(float4(input.positionModel.xyz, 1.f), world).xyz;
	output.positionProjection = mul(float4(output.positionWorld, 1.f), view);
	output.positionProjection = mul(output.positionProjection, proj);
	
	output.texcoordinate = input.texcoordinate;
	output.tangentWorld = mul(float4(input.tangentModel.xyz, 0.f), worldIT).xyz;
	output.positionModel = input.positionModel;
	
	
	return output;
}