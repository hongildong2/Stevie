#pragma once
#ifdef API_D3D11
#include "SubModules/Render/D3D11/D3D11Resources.h"
#define RHIDepthStencilState D3D11DepthStencilState
#endif

class RDepthStencilState final : public RHIDepthStencilState
{
};

