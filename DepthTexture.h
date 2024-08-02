#pragma once
#include "pch.h"
#include "Utility.h"


// RenderPass, RenderResource, RenderObject�� �и�
// RenderPass�� context����, RenderResource�� ���ҽ��� ������ �ִ� ��ü, RenderObject�� ������ ��ü��
// RenderPass.Render() -> SetContexts, RenderResource(context) {set ressource}, for renderObjects, draw obj
// RenderGraph by RenderPass

class IDepthRenderable;

class DepthTexture
{
public:
	DepthTexture(const D3D11_VIEWPORT viewPort)
		: m_viewport(viewPort)
	{
	}

	void Initialize(ID3D11Device1* pDevice);

	inline ID3D11DepthStencilView* GetDSV() const
	{
		return m_depthDSV.Get();
	}

	inline ID3D11ShaderResourceView* GetSRV() const

	{
		return m_depthSRV.Get();
	}

	inline const D3D11_VIEWPORT* GetVP() const
	{
		return &m_viewport;
	}


private:
	// RenderResource		
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthTexture;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthDSV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_depthSRV;

	const D3D11_VIEWPORT m_viewport;
};

