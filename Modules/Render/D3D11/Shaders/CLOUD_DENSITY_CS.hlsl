#include "INCL_GPURandom.hlsli"

SamplerState linearClamp : register(s0);
RWTexture3D<float> densityTex : register(u0);

float cloudDensity(float3 uvw)
{
	float freq = 32.0f;
    
	float pfbm = lerp(1., perlinfbm(uvw, 2., 1), .88);
	pfbm = abs(pfbm * 2.f - 1.f); // billowy perlin noise

	float g = worleyFbm(uvw, freq);
	float r = remap(pfbm, 0.1f, 1.2f, g, 1.1f); // perlin-worley
	float b = worleyFbm(uvw, freq * 2.0f);
	float a = worleyFbm(uvw, freq * 4.0f);

	float perlinWorley = r;
	float wfbm = g * .525f + b * .125f + a * .35f;

    // cloud shape modeled after the GPU Pro 7 chapter
	float cloud = remap(perlinWorley, wfbm - 1., 1., 0., 1.);
	cloud = remap(cloud, .85, 1., 0., 1.); // fake cloud coverage
    
	return saturate(cloud);
}

[numthreads(16, 16, 4)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint width, height, depth;
	densityTex.GetDimensions(width, height, depth);
	
	float3 uvwOffset = float3(0.016, 0.013, 0.02);
    
	float3 uvw = DTid / float3(width, height, depth) + uvwOffset; // 노이즈 생성을 위해 uvwOffset 사용

	densityTex[DTid] = cloudDensity(uvw);
}