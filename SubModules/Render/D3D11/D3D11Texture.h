#pragma once
#include "pch.h"
#include "../RTexture.h"

class D3D11Renderer;


class D3D11Texture2D : public RTexture2D
{
public:
	D3D11Texture2D();
	~D3D11Texture2D() = default;


	virtual BOOL IsDynamicTexture() override;
	void Initialize(const D3D11Renderer* pRenderer, const UINT width, const UINT height, const DXGI_FORMAT format);
	void InitializeFromFile(const D3D11Renderer* pRenderer, const WCHAR* path);


protected:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_UAV;
};


class D3D11Texture3D : public RTexture3D
{
public:
	D3D11Texture3D();
	~D3D11Texture3D() = default;


	virtual BOOL IsDynamicTexture() override;


protected:
	Microsoft::WRL::ComPtr<ID3D11Texture3D> m_texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_UAV;
};