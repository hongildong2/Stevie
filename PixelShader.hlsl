struct PixelShaderInput
{
	float4 posProj : SV_POSITION; // Screen position
	float3 posWorld : POSITION0; // World position (���� ��꿡 ���)
	float3 normalWorld : NORMAL0;
	float2 texcoord : TEXCOORD0;
	float3 tangentWorld : TANGENT0;
	float3 posModel : POSITION1; // Volume casting ������
};


float4 main(PixelShaderInput input) : SV_TARGET
{
	return float4(1.0f, 0.f, 0.1f, 1.0f);
}