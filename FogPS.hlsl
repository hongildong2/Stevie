#include "ScreenSpace.hlsli"
#include "RenderingCommons.hlsli"

Texture2D<float3> RenderResult : register(t100);
Texture2D<float> depthOnlyTex : register(t101);

int RaySphereIntersection(in float3 start, in float3 dir, in float3 center, in float radius,
                            out float t1, out float t2)
{
	float3 p = start - center;
	float pdotv = dot(p, dir);
	float p2 = dot(p, p);
	float r2 = radius * radius;
	float m = pdotv * pdotv - (p2 - r2);
    
	if (m < 0.0)
	{
		t1 = 0;
		t2 = 0;
		return 0;
	}
	else
	{
		m = sqrt(m);
		t1 = -pdotv - m;
		t2 = -pdotv + m;
		return 1;
	}
}


// "Foundations of Game Engine Development" by Eric Lengyel, V2 p319
float HaloEmission(float3 posView, float radius, float3 lightPosViewSpace)
{
    // Halo
	float3 rayStart = float3(0, 0, 0); // View space 눈의 위치
    // 래스터화 된 픽셀의 좌표계를 뷰 영역으로 옮긴것이므로 posView는 눈의 위치가아니라 투영된 픽셀의 뷰 공간 위치
	float3 dir = normalize(posView - rayStart);    

	float3 center = lightPosViewSpace;
	float lightDepth = center.z;
    
    //if (depth < lightDepth - radius)
    //{
    //    return 0.0;
    //}
	float t1 = 0.0;
	float t2 = 0.0;
	float3 p = rayStart - center;
	if (RaySphereIntersection(rayStart, dir, center, radius, t1, t2) && t1 < posView.z)
	{
        //if (depth - 0.001 <= lightDepth)
        //{
        //// 물체가 더 앞에 => t2 becomes depth
        //    t2 = depth;
        //    // else 헤일로가 더 앞에 => full path. keep t2
        //}
		t2 = min(posView.z, t2);
		float t1_2 = t1 * t1;
		float t1_3 = t1_2 * t1;
		float t2_2 = t2 * t2;
		float t2_3 = t2_2 * t2;
		float R2 = radius * radius;
        
        
        
		float emit = (1 - dot(p, p) / R2) * (t2 - t1) - dot(p, dir) * (t2_2 - t1_2) / R2 - dot(dir, dir) *
        (t2_3 - t1_3) / 3.0 / R2;
		emit /= (4 * radius / 3.0);
		return emit;

	}
	else
	{
		return 0.0;
	}
}

float4 main(SamplingPixelShaderInput input) : SV_TARGET
{
	float4 positionViewSpace = TexcoordToView(input.texcoord, depthOnlyTex, linearClamp, invProj);
	float dist = length(positionViewSpace.xyz);
	float3 color = RenderResult.Sample(linearClamp, input.texcoord).rgb;
	
	// halo
	for (uint i = 0; i < globalLightsCount; ++i) // except sun
	{
		float3 haloColor = globalLights[i].color;
		float3 lightPosView = mul(float4(globalLights[i].positionWorld.xyz, 1), view).xyz;
		color += HaloEmission(positionViewSpace.xyz, globalLights[i].haloRadius, lightPosView) * haloColor * globalLights[i].haloStrength;

	}
	
	
	// fog
	float3 fogColor = float3(1, 1, 1);
	
	float fogMin = 5.0;
	float fogMax = 40.0;
	
	float distFog = saturate((dist - fogMin) / (fogMax - fogMin));
	
	float fogStrength = 1.1;
	float fogFactor = exp(-distFog * fogStrength);

	
	
	color = lerp(fogColor, color, fogFactor);
	
	return float4(color, 1.0);
}