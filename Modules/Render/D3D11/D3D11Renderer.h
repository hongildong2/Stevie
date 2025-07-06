#pragma once
#include "pch.h"
#include "D3D11DeviceResources.h"

class D3D11Renderer
{
public:
	D3D11Renderer() = default;
	~D3D11Renderer() = default;

	inline D3D11DeviceResources* GetDeviceResources() const
	{
		return m_deviceResources.get();
	}

protected:

	std::unique_ptr <D3D11DeviceResources> m_deviceResources;


};