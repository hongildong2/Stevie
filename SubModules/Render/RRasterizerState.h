#pragma once

#ifdef API_D3D11
#include "SubModules/Render/D3D11/D3D11Resources.h"
#define RHIRasterizerState D3D11RasterizerState
#endif

class RRasterizerState final : public RHIRasterizerState
{
};

