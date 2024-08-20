#pragma once
#include "pch.h"

class VolumetricTexture final
{
public:
	VolumetricTexture(const DXGI_FORMAT format, const size_t width, const size_t height, const size_t depth);
	~VolumetricTexture() = default;

	VolumetricTexture(const VolumetricTexture& other) = delete;
	VolumetricTexture& operator=(const VolumetricTexture& other) = delete;

	void SetDevice(ID3D11Device* pDevice);
	void ReleaseDevice();
	void Initialize();
	inline ID3D11Texture3D* GetTexture() const
	{
		return m_volumetricTexture.Get();
	}

	inline ID3D11ShaderResourceView* GetSRV() const
	{
		return m_srv.Get();
	}

	inline ID3D11UnorderedAccessView* GetUAV() const
	{
		return m_uav.Get();
	}


private:
	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11Texture3D> m_volumetricTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_uav;

	DXGI_FORMAT m_format;

	size_t m_width;
	size_t m_height;
	size_t m_depth;
};

