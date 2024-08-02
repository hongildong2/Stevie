#pragma once
#include "pch.h"
#include "Utility.h"
#include "DepthTexture.h"
#include "DepthOnlyResources.h"

class IDepthRenderable
{
public:
	IDepthRenderable()
		: m_depthTex()
	{
		DepthOnlyResources::OnRegisterDepthOnlyObject(this);
	}

	virtual ~IDepthRenderable()
	{
		DepthOnlyResources::OnDestoryDepthOnlyObject(this);
	}

	virtual DepthOnlyConstant GetDepthOnlyConstant() const = 0;

	// Render() in RenderPass
	virtual void SetContextDepthOnly(ID3D11DeviceContext1* pContext)
	{
		pContext->ClearDepthStencilView(m_depthTex->GetDSV(), D3D11_CLEAR_DEPTH, 1.0f, 0); // set RTV to NULL, Depth Only
		pContext->OMSetRenderTargets(0, NULL, m_depthTex->GetDSV());

		if (m_depthTex->GetVP() != NULL)
		{
			pContext->RSSetViewports(1, m_depthTex->GetVP());
		}

		DepthOnlyConstant temp = GetDepthOnlyConstant();
		Utility::DXResource::UpdateConstantBuffer(temp, pContext, DepthOnlyResources::depthOnlyCB);
		pContext->VSSetConstantBuffers(3, 1, DepthOnlyResources::depthOnlyCB.GetAddressOf());
	}

protected:
	std::unique_ptr<DepthTexture> m_depthTex;
};