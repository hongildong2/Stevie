#pragma once
#include "pch.h"
#include "../RTexture.h"
#include "D3D11Resources.h"

// CONSIDER :: Consider One big class and give Interface to access textures
/*
* class D3D11Texture : public RTexture, public IDepthTexture, public IRenderTexture, public IDynamicTexture, public IReadonlyTexture
*
*/

class D3D11Renderer;
class D3D11Texture : public RTexture
{
	friend class D3D11ResourceManager;
public:
	D3D11Texture(ETextureType type);
	~D3D11Texture() = default;

	void Initialize(const UINT width, const UINT height, const UINT depth, const DXGI_FORMAT format, const BOOL bReadOnly);


	inline ID3D11ShaderResourceView* GetSRVOrNull() const
	{
		return m_SRV.Get();
	}

	inline ID3D11UnorderedAccessView* GetUAVOrNull() const // Non-Dynamic texture has no UAV
	{
		MY_ASSERT(m_bIsReadOnly == FALSE);
		return m_UAV.Get();
	}

protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_UAV;
};

class D3D11TextureDepth final : public D3D11Texture, public D3D11Resource<ID3D11Texture2D>
{
	friend class D3D11ResourceManager;
public:
	D3D11TextureDepth();
	~D3D11TextureDepth() = default;

	inline ID3D11DepthStencilView* GetDSV() const
	{
		return m_DSV.Get();
	}

private:

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DSV;
};

class D3D11TextureRender final : public D3D11Texture, public D3D11Resource<ID3D11Texture2D>
{
	friend class D3D11ResourceManager;
public:
	D3D11TextureRender(const DXGI_FORMAT format);
	~D3D11TextureRender() = default;

	inline ID3D11RenderTargetView* GetRTV() const
	{
		return m_RTV.Get();
	}

private:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RTV;
};

class D3D11Texture2D : public D3D11Texture, public D3D11Resource<ID3D11Texture2D>
{
	friend class D3D11ResourceManager;
public:
	D3D11Texture2D();
	~D3D11Texture2D() = default;
};


class D3D11Texture3D : public D3D11Texture, public D3D11Resource<ID3D11Texture3D>
{
	friend class D3D11ResourceManager;
public:
	D3D11Texture3D();
	~D3D11Texture3D() = default;
};

class D3D11TextureCube : public D3D11Texture
{
	friend class D3D11ResourceManager;
public:
	D3D11TextureCube();
	~D3D11TextureCube() = default;
};