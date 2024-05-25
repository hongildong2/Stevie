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

struct Material
{
	float3 ambient;
	float shininess;
	float3 diffuse;
	float dummy1;
	float3 specular;
	float dummy2;
	float3 fresnelR0;
	float dummy3;
};

float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, Material mat)
{
	float3 halfway = normalize(toEye + lightVec);
	float hdotn = dot(halfway, normal);
	float3 specular = mat.specular * pow(max(hdotn, 0.f), mat.shininess);
	
	return mat.ambient + (mat.diffuse + specular) * lightStrength;
}

float3 ComputeDirectionalLight(Light L, Material mat, float3 normal, float3 toEye)
{
	float3 lightVec = -L.direction;

	float ndotl = max(dot(lightVec, normal), 0.0f);
	float3 lightStrength = L.strength * ndotl;

    // Luna DX12 å������ Specular ��꿡��
    // Lambert's law�� ����� lightStrength�� ����մϴ�.
	return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

#endif // __COMMON_HLSLI__