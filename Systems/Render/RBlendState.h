#pragma once

#ifdef API_D3D11
#include "Systems/Render/D3D11/D3D11Resources.h"
#define RHIBlendState D3D11BlendState
#endif

class RBlendState : public RHIBlendState
{
};

