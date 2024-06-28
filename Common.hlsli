#ifndef __COMMON_HLSLI__
#define __COMMON_HLSLI__
/*
	From honglab's introduction to graphics course
*/

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

struct Light
{
	float3 strength;
	float fallOffStart;
	float3 direction;
	float fallOffEnd;
	float3 position;
	float spotPower;
};

static const float PI = 3.14159265359;

#endif // __COMMON_HLSLI__