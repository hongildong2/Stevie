// https:github.com/gasgiant/FFT-Ocean/blob/main/Assets/ComputeShaders/FastFourierTransform.compute
// https:github.com/gasgiant/Ocean-URP/blob/main/Assets/OceanSystem/Shaders/Resources/ComputeShaders/FFT.compute

#include "Common.hlsli"
#include "OceanFunctions.hlsli"

static uint size = SIZE;

// z indexed wave cascades
RWTexture2DArray<float4> FTResultTex;

cbuffer FFTInfo : register(b0)
{
	uint targetCount;
	bool direction;
	bool inverse;
	bool scale;
	bool permute;
}



// Inverse는 그냥 계수 sign만 살짝 달라지고 연산은 동일
// 논문에만 x,z 벡터고 연산은 그냥 서로 독립적으로 진행하면 됨
// Bit reversion ordering이 곤란하다
// 솔직히 GPU FFT를 잘 모르겠어요...

groupshared float4 buffer[2][SIZE]; // N/2 <-> N 번갈아가면서 버퍼역할 바꾸기

float2 ComplexMult(float2 a, float2 b)
{
	return float2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

void ButterflyValues(uint step, uint index, out uint2 indices, out float2 twiddle)
{
	const float twoPi = 6.28318530718;
	uint b = size >> (step + 1); // step이 logged 되었으
	uint w = b * (index / b); // frquency basis selection, ki of 2DIT radix case
	uint i = (w + index) % size; // Xk 에서, k의 인덱싱으로부터 xn,xm, 인풋값의  인덱스를 역추적, k = 0 -> n = 0, m = 1. 왜 식이 이렇게되는지는 나한테 묻지마라..e
	sincos(-twoPi / size * w, twiddle.y, twiddle.x);
	if (inverse)
		twiddle.y = -twiddle.y;
	indices = uint2(i, i + b); // 
}

float4 DoFft(uint threadIndex, float4 input)
{
	buffer[0][threadIndex] = input;
	GroupMemoryBarrierWithGroupSync();
	bool flag = false;
    
    [unroll(LOG_SIZE)]
	for (uint step = 0; step < LOG_SIZE; step++) // N = 1 -> 2 , 4, 6 .... UNTIL SIZE
	{
		uint2 inputsIndices;
		float2 twiddle;
		ButterflyValues(step, threadIndex, inputsIndices, twiddle);
        
		float4 v = buffer[flag][inputsIndices.y];
		buffer[!flag][threadIndex] = buffer[flag][inputsIndices.x]
		    + float4(ComplexMult(twiddle, v.xy), ComplexMult(twiddle, v.zw));
		flag = !flag; // ping pong
		GroupMemoryBarrierWithGroupSync();
	}
    
	return buffer[flag][threadIndex]; // one line FFT done!
}


// As we calculated time-dependent wave spectrum, N = 1 is already calculated, we do IFFT from there.
[numthreads(SIZE, 1, 1)]
void FFT( uint3 DTid : SV_DispatchThreadID ) // this input index is not bit reversed
{
	uint threadIndex = DTid.x;
	uint2 targetIndex;
	if (direction) // vert, horizontal
		targetIndex = DTid.yx;
	else
		targetIndex = DTid.xy;
    

    for (uint k = 0; k < TARGET_COUNT; k++)
    {
        FTResultTex[uint3(targetIndex, k)] = DoFft(threadIndex, FTResultTex[uint3(targetIndex, k)]);
    }

}


float4 DoPostProcess(float4 input, uint2 id)
{
	if (scale)
		input /= size * size;
	if (permute)
		input *= 1.0 - 2.0 * ((id.x + id.y) % 2);
	return input;
}

// wave post process
[numthreads(16, 16, 4)]
void PostProcess(uint3 id : SV_DispatchThreadID)
{
    FTResultTex[id] = DoPostProcess(FTResultTex[id], id.xy);
    
}