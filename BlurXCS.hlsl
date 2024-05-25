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
	uint width, height;
	input.GetDimensions(width, height);

	// DTid°¡ ÇÈ¼¿ ÁÂÇ¥ ±×ÀÚÃ¼
	float3 blurred = float3(0, 0, 0);
	
	[unroll]
	for (int i = -blurRadius; i <= blurRadius; ++i)
	{
		// bound x coordinate in 
		uint boundX = clamp(i + DTid.x, 0, width - 1);
		float3 sampled = input[uint2(boundX, DTid.y)].xyz; // ¹é¹öÆÛ¶û °°ÀÌ¸ÂÃç¼­ float4
		
		blurred += weights[i + blurRadius] * sampled;
	}
	
	output[DTid.xy] = float4(blurred, 1);
	
}