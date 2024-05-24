struct PixelShaderInput
{
	float4 positionProjection : SV_POSITION; // Screen position
	float3 positionWorld : POSITION0; // World position (조명 계산에 사용)
	float3 normalWorld : NORMAL0;
	float2 texcoordinate : TEXCOORD0;
	float3 tangentWorld : TANGENT0;
	float3 positionModel : POSITION1; // Volume casting 시작점
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

cbuffer Lights : register(b1)
{
	Light dirLight;
	Light pointLight;
	Light spotLight;
};

cbuffer PSConstant : register(b2)
{
	float3 eyeWorld;
	bool useTexture;
	Material material;
	float3 rimColor;
	float rimPower;
	float rimStrength;
	bool useSmoothStep;
	float2 dummy;
};

TextureCube cubeMap : register(t0);
Texture2D albedo : register(t1);

SamplerState linearSampler : register(s0);


float4 main(PixelShaderInput input) : SV_TARGET
{
	float3 toCube = reflect(-normalize(eyeWorld - input.positionWorld), input.normalWorld);
	float a = dot(input.normalWorld, dirLight.direction) * dirLight.strength;
	float4 color = cubeMap.Sample(linearSampler, toCube);
	return color;
	//return float4(1.f, 0.f, 0.f, 1.0f);
}