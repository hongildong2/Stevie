
#ifndef __GPU_RANDOM_HLSLI__
#define __GPU_RANDOM_HLSLI__

static const float GPU_PI = 3.14159265359;

uint baseHash(uint3 p)
{
	p = 1103515245U * ((p.xyz >> 1U) ^ (p.yzx));
	uint h32 = 1103515245U * ((p.x ^ p.z) ^ (p.y >> 3U));
	return h32 ^ (h32 >> 16);
}

float hash13(uint3 x)
{
	uint n = baseHash(x);
	return float(n) * (1.0 / float(0xffffffffU));
}

float2 hash23(float3 x)
{
	uint n = baseHash(x);
	uint2 rz = uint2(n, n * 48271U); //see: httprandom.mat.sbg.ac.at/results/karl/server/node4.html
	return float2(rz.xy & (uint2) 0x7fffffffU) / float(0x7fffffff);
}

float3 hash33(uint3 x)
{
	uint n = baseHash(x);
	uint3 rz = uint3(n, n * 16807U, n * 48271U); //see: http:random.mat.sbg.ac.at/results/karl/server/node4.html
	return float3(rz & (uint3) 0x7fffffffU) / float(0x7fffffff);
}

float NormalRandom(uint3 id)
{
	float2 random01 = hash23(id);
	return cos(2 * GPU_PI * random01.x) * sqrt(-2 * log(random01.y));
}

#endif // ASD