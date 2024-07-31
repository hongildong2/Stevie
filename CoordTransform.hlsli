#ifndef __COORD_TRANSFORM__
#define __COORD_TRANSFORM__


struct SamplingVertexShaderInput
{
	float3 position : POSITION;
	float2 texcoord : TEXCOORD;
};

struct SamplingPixelShaderInput
{
	float3 positionModel : POSITION;
	float4 positionProj : SV_POSITION; // 여기에 뭔 값이라도 넣어줘야 파이프라인에서 래스터를 하지!! screen quad는 그냥 우연으로 됐던것 뿐
	float2 texcoord : TEXCOORD;
};

float4 TexcoordToView(float2 texcoord, Texture2D<float> depthOnlyTex, SamplerState linearClamp, matrix invProj)
{
	float4 posProj;

    // [0, 1]x[0, 1] -> [-1, 1]x[-1, 1] from texcoord to NDC
	posProj.xy = texcoord * 2.0 - 1.0;
	posProj.y *= -1; // y 좌표 뒤집기
	posProj.z = depthOnlyTex.Sample(linearClamp, texcoord).r;
	posProj.w = 1.0;
	// NDC now recovered
    
    // ProjectSpace -> ViewSpace
    //float4 posView = mul(posProj, lights[0].invProj);
	float4 posView = mul(posProj, invProj);
	posView.xyz /= posView.w;
    
	// view Pos now recovered
	return posView;
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

#endif /* __COORD_TRANSFORM__ */