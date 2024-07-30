#pragma once
#include "pch.h"

class IWindowSizeDependent
{
	virtual void OnWindowSizeChange(ID3D11Device1* pDevice, RECT size) = 0;
};