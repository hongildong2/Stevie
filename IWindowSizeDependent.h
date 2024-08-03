#pragma once
#include "pch.h"

class IWindowSizeDependent
{
	virtual void OnWindowSizeChange(ID3D11Device1* pDevice, D3D11_VIEWPORT vp, DXGI_FORMAT bufferFormat) = 0;
};