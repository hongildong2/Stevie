#ifndef __COORDINATE_TRANSFORM__
#define __COORDINATE_TRANSFORM__

float4 TexcoordToView(float2 texcoord, Texture2D<float> depthOnlyTex, SamplerState linearClamp, matrix invProj)
{
	float4 posProj;

    // [0, 1]x[0, 1] -> [-1, 1]x[-1, 1] from texcoord to NDC
	posProj.xy = texcoord * 2.0 - 1.0;
	posProj.y *= -1; // y 좌표 뒤집기
	posProj.z = saturate(depthOnlyTex.Sample(linearClamp, texcoord).r);
	posProj.w = 1.0;
	// NDC now recovered
    
    // ProjectSpace -> ViewSpace
    //float4 posView = mul(posProj, lights[0].invProj);
	float4 posView = mul(posProj, invProj);
	posView.xyz /= posView.w;
    
	// view Pos now recovered
	return posView;
}

float2 ScreenToTexcoord(float2 positionScreen)
{
	float2 texcoord = float2(positionScreen.x, -positionScreen.y);
	texcoord += 1.0;
	texcoord *= 0.5;
	
	return texcoord;
}

float NDCToViewDepth(float NDCDepth, matrix invProj)
{
	float4 pointView = mul(float4(0, 0, NDCDepth, 1), invProj); // into view space
	return pointView.z / pointView.w;

}
// to sample skymap from texture cube
float3 GetSkyCubeNormal(float2 uv)
{
	// normal to sky, y is always 1 xz -11 -> 00, xz 1-1 -> 11
	const float3 UNIT_Y = float3(0, 1, 0);
	float2 xzVector = (uv * 2.0) - float2(1, 1);
	xzVector.y = -xzVector.y;
	float3 xzUnitVec = float3(xzVector.x, 0, xzVector.y);
	
	float3 uvToNormal = normalize(xzUnitVec + UNIT_Y);
	
	return uvToNormal;
}

float3 GetNormalTexture(in
Texture2D<float3> normalTex, in
SamplerState linearWrapSS, in
bool bInvertNormalMapY, in
float3 normalWorld, in
float3 tangentWorld, in
float2 uv)
{
	float3 normal = normalTex.Sample(linearWrapSS, uv).rgb;
	normal = 2.0 * normal - 1.0; // 범위 조절 [-1.0, 1.0]

    // OpenGL 용 노멀맵일 경우에는 y 방향을 뒤집어줍니다.
	// normal.y = bInvertNormalMapY ? -normal.y : normal.y;
        
	float3 N = normalize(normalWorld);
	float3 T = normalize(tangentWorld - dot(tangentWorld, N) * N);
	float3 B = cross(N, T);
        
    
	float3x3 TBN = float3x3(T, B, N); // matrix는 float4x4, 여기서는 벡터 변환용이라서 3x3 사용
	
	return normalize(mul(normal, TBN));
}


#endif /* __COORDINATE_TRANSFORM__ */