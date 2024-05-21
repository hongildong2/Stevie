struct VertexShaderInput
{
	float3 positionModel : POSITION; //모델 좌표계의 위치 position
	float3 normalModel : NORMAL; // 모델 좌표계의 normal    
	float2 texcoordinate : TEXCOORD;
	float3 tangentModel : TANGENT;
};

struct PixelShaderInput
{
	float4 positionProjection : SV_POSITION; // Screen position
	float3 positionWorld : POSITION0; // World position (조명 계산에 사용)
	float3 normalWorld : NORMAL0;
	float2 texcoordinate : TEXCOORD0;
	float3 tangentWorld : TANGENT0;
	float3 positionModel : POSITION1; // Volume casting 시작점
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