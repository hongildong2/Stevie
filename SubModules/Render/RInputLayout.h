#pragma once

#ifdef API_D3D11
#include "SubModules/Render/D3D11/D3D11Resources.h"
#define RHIInputLayout D3D11InputLayout
#endif


class RInputLayout final : public RHIInputLayout
{
};

