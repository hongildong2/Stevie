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

	std::unique_ptr<RBuffer> m_globalCB;
	std::unique_ptr<RBuffer> m_sunLightCB;

	std::unique_ptr<RBuffer> m_meshCB;
	std::unique_ptr<RBuffer> m_materialCB;
	std::unique_ptr<RBuffer> m_computeCB;
};