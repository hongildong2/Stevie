#include "pch.h"
#include "Utility.h"

namespace Utility
{
	void ComputeShaderBarrier(ID3D11DeviceContext1* context)
	{
		ID3D11ShaderResourceView* nullSRV[6] = { 0, };
		context->CSSetShaderResources(0, 6, nullSRV);
		ID3D11UnorderedAccessView* nullUAV[6] = { 0, };
		context->CSSetUnorderedAccessViews(0, 6, nullUAV, NULL);
	}
}