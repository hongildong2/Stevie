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

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_globalCB;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_sunLightCB;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_meshCB;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_materialCB;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_computeCB;
};