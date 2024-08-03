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
		auto* dor = DepthOnlyResources::GetInstance();
		dor->OnRegisterDepthOnlyObject(this);
	}

	virtual ~IDepthRenderable()
	{
		auto* dor = DepthOnlyResources::GetInstance();
		dor->OnDestoryDepthOnlyObject(this);
	}

	virtual DirectX::SimpleMath::Matrix GetViewRow() const = 0;
	virtual DirectX::SimpleMath::Matrix GetProjRow() const
	{
		return m_proj;
	}

	virtual void Initialize(ID3D11Device1* pDevice) const
	{
		m_depthTex->Initialize(pDevice);
	}


	inline ID3D11ShaderResourceView* GetDepthBufferSRV() const
	{
		return IDepthRenderable::m_depthTex->GetSRV();
	}


	void GetDepthOnlyConstantColumn(DepthOnlyConstant* outDepthOnlyConstantColumnWise) const
	{
		assert(outDepthOnlyConstantColumnWise != nullptr);
		outDepthOnlyConstantColumnWise->viewCol = GetViewRow().Transpose();
		outDepthOnlyConstantColumnWise->projCol = IDepthRenderable::m_proj.Transpose();
	}

	// Render() in RenderPass
	void SetContextDepthOnly(ID3D11DeviceContext1* pContext) const
	{
		pContext->ClearDepthStencilView(m_depthTex->GetDSV(), D3D11_CLEAR_DEPTH, 1.0f, 0); // set RTV to NULL, Depth Only
		pContext->OMSetRenderTargets(0, NULL, m_depthTex->GetDSV());

		if (m_depthTex->GetVP() != NULL)
		{
			pContext->RSSetViewports(1, m_depthTex->GetVP());
		}

		DepthOnlyConstant temp;
		GetDepthOnlyConstantColumn(&temp);
		auto* dor = DepthOnlyResources::GetInstance();
		auto a = dor->GetDepthOnlyCB();
		ID3D11Buffer* cbs[1] = { a.Get() };
		Utility::DXResource::UpdateConstantBuffer(temp, pContext, a);
		pContext->VSSetConstantBuffers(3, 1, cbs);
	}

protected:
	std::unique_ptr<DepthTexture> m_depthTex;
	DirectX::SimpleMath::Matrix m_proj;
};