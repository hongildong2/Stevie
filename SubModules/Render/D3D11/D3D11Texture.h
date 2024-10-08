#pragma once
#include "pch.h"
#include "../RTexture.h"
#include "D3D11Resources.h"

class D3D11Renderer;


class D3D11Texture2D : public RTexture2D, public D3D11Resource<ID3D11Texture2D>
{
public:
	D3D11Texture2D() = default;
	~D3D11Texture2D() = default;


	void Initialize(const D3D11Renderer* pRenderer, const UINT width, const UINT height, const DXGI_FORMAT format, const BOOL bIsDynamic);
	void InitializeFromFile(const D3D11Renderer* pRenderer, const WCHAR* path);


protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_UAV;
};


class D3D11Texture3D : public RTexture3D, public D3D11Resource<ID3D11Texture3D>
{
public:
	D3D11Texture3D() = default;
	~D3D11Texture3D() = default;

	void Initialize(const D3D11Renderer* pRenderer, const UINT width, const UINT height, const UINT depth, const DXGI_FORMAT format, const BOOL bIsDynamic);


protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_UAV;
};