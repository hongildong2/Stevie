// https:github.com/gasgiant/FFT-Ocean/blob/main/Assets/ComputeShaders/FastFourierTransform.compute
// https:github.com/gasgiant/Ocean-URP/blob/main/Assets/OceanSystem/Shaders/Resources/ComputeShaders/FFT.compute

#include "OceanGlobal.hlsli"

static uint size = SIZE;

// z indexed wave cascades
RWTexture2DArray<float4> FTResultTex : register(u0);

cbuffer FFTInfo : register(b0)
{
	uint targetCount;
	bool direction;
	bool inverse;
	bool scale;
	
	bool permute;
	float3 dummy;
}



// Inverse�� �׳� ��� sign�� ��¦ �޶����� ������ ����
// ������ x,z ���Ͱ� ������ �׳� ���� ���������� �����ϸ� ��
// Bit reversion ordering�� ����ϴ�

groupshared float4 buffer[2][FFT_SIZE]; // N/2 <-> N �����ư��鼭 ���ۿ��� �ٲٱ�

float2 ComplexMult(float2 a, float2 b)
{
	// b�� ������, ���ϱ� ����� float������ �Ѿ�� NaN�ΰ�? �ʹ����� ������ 0ó���� �ϴ°� ������
	return float2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

void ButterflyValues(uint step, uint index, out uint2 indices, out float2 twiddle)
{
	const float twoPi = 6.28318530718;
	uint b = size >> (step + 1); // step�� logged �Ǿ���
	uint w = b * (index / b); // frquency basis selection, ki of 2DIT radix case
	uint i = (w + index) % size; // Xk ����, k�� �ε������κ��� xn,xm, ��ǲ����  �ε����� ������, k = 0 -> n = 0, m = 1. �� ���� �̷��ԵǴ����� ������ ��������..e
	sincos(-twoPi / (float) size * (float) w, twiddle.y, twiddle.x);
	if (inverse)
		twiddle.y = -twiddle.y;
	indices = uint2(i, i + b); // 
}

float4 DoFft(uint threadIndex, float4 input)
{
	buffer[0][threadIndex] = input; // data filling
	GroupMemoryBarrierWithGroupSync();
	bool flag = false;
    
    // [unroll(LOG_SIZE)]
	for (uint step = 0; step < LOG_SIZE; step++) // N = 1 -> 2 , 4, 6 .... UNTIL SIZE
	{
		uint2 inputsIndices;
		float2 twiddle;
		ButterflyValues(step, threadIndex, inputsIndices, twiddle);
		
		
		float4 v = buffer[flag][inputsIndices.y];
		
		buffer[!flag][threadIndex] = buffer[flag][inputsIndices.x] + float4(ComplexMult(twiddle, v.xy), ComplexMult(twiddle, v.zw));
			
		flag = !flag; // ping pong
		GroupMemoryBarrierWithGroupSync();
	}
    
	return buffer[flag][threadIndex]; // one line FFT done!
}


// As we calculated time-dependent wave spectrum, N = 1 is already calculated, we do IFFT from there.
[numthreads(FFT_SIZE, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID ) // this input index is not bit reversed
{
	const uint threadIndex = DTid.x;
	uint2 targetIndex;
	if (direction)
		targetIndex = DTid.yx; // vertical
	else
		targetIndex = DTid.xy; // horizontal
    
	// [unroll(TARGET_COUNT)] :: Shader compile too slow
	for (uint k = 0; k < CASCADE_COUNT; k++)
    {
        FTResultTex[uint3(targetIndex, k)] = DoFft(threadIndex, FTResultTex[uint3(targetIndex, k)]);
    }

}
