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

cbuffer transform : register(b0)
{
	float4x4 world;
	float4x4 worldIT;
	float4x4 view;
	float4x4 proj;
};

PixelShaderInput main(VertexShaderInput input)
{
    // 불필요한 멤버들도 VertexShaderInput을 통일시켰기 때문에 채워줘야 합니다.
    
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
	output.positionProjection= pos;

	output.texcoordinate = input.texcoordinate;

	return output;
}
