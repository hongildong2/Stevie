#ifndef __SCREEN_SPACE__
#define __SCREEN_SPACE__


struct SamplingVertexShaderInput
{
	float3 position : POSITION;
	float2 texcoord : TEXCOORD;
};

struct SamplingPixelShaderInput
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

float4 TexcoordToView(float2 texcoord, Texture2D<float> depthOnlyTex, SamplerState linearClamp, matrix invProj)
{
	float4 posProj;

    // [0, 1]x[0, 1] -> [-1, 1]x[-1, 1] from texcoord to NDC
	posProj.xy = texcoord * 2.0 - 1.0;
	posProj.y *= -1; // y ÁÂÇ¥ µÚÁý±â
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

#endif /* __SCREEN_SPACE_HLSLI__ */