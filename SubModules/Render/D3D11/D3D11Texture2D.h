#pragma once
#include "pch.h"
#include "../RTexture2D.h"

class D3D11Renderer;
class D3D11Texture2D : public RTexture2D
{
public:
	D3D11Texture2D();
	~D3D11Texture2D() = default;


	void Initialize(const D3D11Renderer* pRenderer, const UINT width, const UINT height, const DXGI_FORMAT format);
	void InitializeFromFile(const D3D11Renderer* pRenderer, const WCHAR* path);


protected:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SRV;
};

