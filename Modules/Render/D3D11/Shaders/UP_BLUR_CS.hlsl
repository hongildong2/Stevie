Texture2D<float4> input : register(t0);
RWTexture2D<float4> output : register(u0);

// https://learnopengl.com/Guest-Articles/2022/Phys.-Based-Bloom

SamplerState linearClamp : register(s0);

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint width, height;
	output.GetDimensions(width, height);
	
	float dx = 1.0 / (float)width;
	float dy = 1.0 / (float)height;
	
	float2 uv = float2((DTid.x + 1) * dx, (DTid.y + 1) * dy);
	
	float3 a = input.SampleLevel(linearClamp, float2(uv.x - dx, uv.y + dy), 0).rgb;
	float3 b = input.SampleLevel(linearClamp, float2(uv.x, uv.y + dy), 0).rgb;
	float3 c = input.SampleLevel(linearClamp, float2(uv.x + dx, uv.y + dy), 0).rgb;
	
	float3 d = input.SampleLevel(linearClamp, float2(uv.x - dx, uv.y), 0).rgb;
	float3 e = input.SampleLevel(linearClamp, float2(uv.x, uv.y), 0).rgb;
	float3 f = input.SampleLevel(linearClamp, float2(uv.x + dx, uv.y), 0).rgb;
	
	float3 g = input.SampleLevel(linearClamp, float2(uv.x - dx, uv.y - dy), 0).rgb;
	float3 h = input.SampleLevel(linearClamp, float2(uv.x, uv.y - dy), 0).rgb;
	float3 i = input.SampleLevel(linearClamp, float2(uv.x + dx, uv.y - dy), 0).rgb;

	float3 upsampled = e * 4.0;
	upsampled += (b + d + f + h) * 2.0;
	upsampled += (a + c + g + i);
	upsampled *= 1.0 / 16.0;
	output[DTid.xy] = float4(upsampled, 1.0);
}