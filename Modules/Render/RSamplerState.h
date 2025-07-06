#pragma once

#ifdef API_D3D11
#include "Modules/Render/D3D11/D3D11Resources.h"
#define RHISamplerState D3D11SamplerState
#endif

class RSamplerState final : public RHISamplerState
{
};

