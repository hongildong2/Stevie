Texture2D<float4> input : register(t100);
RWTexture2D<float4> output : register(u0);

// https://learnopengl.com/Guest-Articles/2022/Phys.-Based-Bloom

SamplerState linearClamp : register(s0);

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint width, height;
	output.GetDimensions(width, height);
	
	float3 downsampled = 0.0;
	
	float dx = 1.0 / (float)width;
	float dy = 1.0 / (float)height;
	// input DTid�� Read�ϴ� �ؽ��İ� �ƴ� Write�ϴ� �ؽ��� �������� �����ϵ���
	float2 uv = float2((DTid.x + 0.5) * dx, (DTid.y + 0.5) * dy); // �̰��̿��ؼ� �� ū �ؽ��� linearClamp sampling�ϱ�
	
	// �갡 ���� ���� �� �����Ѱ�, Ŀ�θ���� �̷��� �ؽ��� ��ǥ�� �����ϸ��
	float3 a = input.SampleLevel(linearClamp, float2(uv.x - 2.0 * dx, uv.y + 2.0 * dy), 0).rgb;
	float3 b = input.SampleLevel(linearClamp, float2(uv.x, uv.y + 2.0 * dy), 0).rgb;
	float3 c = input.SampleLevel(linearClamp, float2(uv.x + 2.0 * dx, uv.y + 2.0 * dy), 0).rgb;
	
	float3 d = input.SampleLevel(linearClamp, float2(uv.x - 2.0 * dx, uv.y), 0).rgb;
	float3 e = input.SampleLevel(linearClamp, float2(uv.x, uv.y), 0).rgb;
	float3 f = input.SampleLevel(linearClamp, float2(uv.x + 2.0 * dx, uv.y), 0).rgb;
	
	float3 g = input.SampleLevel(linearClamp, float2(uv.x - 2.0 * dx, uv.y - 2.0 * dy), 0).rgb;
	float3 h = input.SampleLevel(linearClamp, float2(uv.x, uv.y - 2.0 * dy), 0).rgb;
	float3 i = input.SampleLevel(linearClamp, float2(uv.x + 2.0 * dx, uv.y - 2.0 * dy), 0).rgb;
	
	float3 j = input.SampleLevel(linearClamp, float2(uv.x - dx, uv.y + dy), 0).rgb;
	float3 k = input.SampleLevel(linearClamp, float2(uv.x + dx, uv.y + dy), 0).rgb;
	float3 l = input.SampleLevel(linearClamp, float2(uv.x - dx, uv.y - dy), 0).rgb;
	float3 m = input.SampleLevel(linearClamp, float2(uv.x + dx, uv.y - dy), 0).rgb;
	
	downsampled = e * 0.125;
	downsampled += (a + c + g + i) * 0.03125;
	downsampled += (b + d + f + h) * 0.0625;
	downsampled += (j + k + l + m) * 0.125;
		
	output[DTid.xy] = float4(downsampled, 1.0);
}