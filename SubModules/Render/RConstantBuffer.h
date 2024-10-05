#pragma once
#include "IRenderer.h"
class RConstantBuffer : public IRenderResource
{
public:
	RConstantBuffer() = default;
	~RConstantBuffer() = default;

	virtual BOOL UpdateBuffer(const void* pInData, const UINT size) = 0;
private:
	const UINT m_bufferSize;
};

