#pragma once
#include "pch.h"
#include "D3D11Resources.h"

class D3D11Texture : public D3D11Resource<ID3D11Resource>
{
public:
	D3D11Texture() = default;
	virtual ~D3D11Texture() = default;

	virtual bool IsInitialized() const = 0;

	inline ID3D11ShaderResourceView* GetSRV() const
	{
		MY_ASSERT(TRUE == IsInitialized());
		return m_SRV.Get();
	}
	inline ID3D11UnorderedAccessView* GetUAV() const
	{
		MY_ASSERT(TRUE == IsInitialized());
		MY_ASSERT(m_UAV != nullptr); // CALLED ON NON DYNAMIC TEX
		return m_UAV.Get();
	}
	inline ID3D11RenderTargetView* GetRTV() const
	{
		MY_ASSERT(TRUE == IsInitialized());
		MY_ASSERT(m_RTV != nullptr); // CALLED ON NON RENDER TEX
		return m_RTV.Get();
	}
	inline ID3D11DepthStencilView* GetDSV() const
	{
		MY_ASSERT(TRUE == IsInitialized());
		MY_ASSERT(m_DSV != nullptr); // CALLED ON NON DEPTH TEX
		return m_DSV.Get();
	}


protected:
	ComPtr<ID3D11ShaderResourceView> m_SRV;
	ComPtr<ID3D11UnorderedAccessView> m_UAV;
	ComPtr<ID3D11DepthStencilView> m_DSV;
	ComPtr<ID3D11RenderTargetView> m_RTV;
};