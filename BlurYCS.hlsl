Texture2D<float4> input : register(t0);
RWTexture2D<float4> output : register(u0);

static const float weights[11] =
{
	0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f,
};

static const int blurRadius = 5;
[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	float3 blurred = float3(0, 0, 0);
	
	uint width, height;
	input.GetDimensions(width, height);
	
	
	for (int i = -blurRadius; i <= blurRadius; ++i)
	{
		uint boundY = clamp(DTid.y + i, 0, height - 1);
		float3 sampled = input[uint2(DTid.x, boundY)].rgb;
		
		blurred += weights[i + blurRadius] * sampled;
	}
		
	output[uint2(DTid.x, DTid.y)] = float4(blurred, 1);
}