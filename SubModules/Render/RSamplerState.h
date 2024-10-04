#pragma once
#include "IRenderer.h"
class RSamplerState : public IRenderResource
{
public:
	RSamplerState(const SAMPLER_DESC& desc);
	~RSamplerState();

private:
	SAMPLER_DESC m_desc;
};

