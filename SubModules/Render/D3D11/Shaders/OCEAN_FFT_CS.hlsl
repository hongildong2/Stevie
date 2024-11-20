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



// Inverse는 그냥 계수 sign만 살짝 달라지고 연산은 동일
// 논문에만 x,z 벡터고 연산은 그냥 서로 독립적으로 진행하면 됨
// Bit reversion ordering이 곤란하다

groupshared float4 buffer[2][FFT_SIZE]; // N/2 <-> N 번갈아가면서 버퍼역할 바꾸기

float2 ComplexMult(float2 a, float2 b)
{
	// b가 작을때, 곱하기 결과가 float범위를 넘어서면 NaN인가? 너무작은 값들은 0처리를 하는게 나을듯
	return float2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

void ButterflyValues(uint step, uint index, out uint2 indices, out float2 twiddle)
{
	const float twoPi = 6.28318530718;
	uint b = size >> (step + 1); // step이 logged 되었으
	uint w = b * (index / b); // frquency basis selection, ki of 2DIT radix case
	uint i = (w + index) % size; // Xk 에서, k의 인덱싱으로부터 xn,xm, 인풋값의  인덱스를 역추적, k = 0 -> n = 0, m = 1. 왜 식이 이렇게되는지는 나한테 묻지마라..e
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
