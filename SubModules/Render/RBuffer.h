#pragma once
#include "pch.h"

#ifdef API_D3D11
#define RHIBuffer D3D11Buffer
#include "SubModules/Render/D3D11/D3D11Resources.h"
#endif

class RBuffer : public RHIBuffer
{
};

