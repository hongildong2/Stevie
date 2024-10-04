#pragma once
#include "IRenderer.h"
class RConstantBuffer : public IRenderResource
{
public:
	RConstantBuffer(const UINT size);
	~RConstantBuffer();

	BOOL UpdateBuffer(const void* pInData, const UINT size);
private:
	const UINT m_bufferSize;
};

