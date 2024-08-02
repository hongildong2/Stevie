#include "pch.h"
#include "DepthOnlyResources.h"
#include "Utility.h"

DepthOnlyResources* DepthOnlyResources::mInstance = nullptr;

DepthOnlyResources::DepthOnlyResources()
	: m_depthOnlyCB()
	, m_savedVPCount(10u)
	, m_savedDSV(nullptr)
	, m_savedRTVs{ nullptr, }
	, m_savedVPs{}
	, m_depthOnlyObjects()
{
}

void DepthOnlyResources::InitDepthOnlyResources(ID3D11Device1* pDevice)
{
	ZeroMemory(&m_savedRTVs, sizeof(ID3D11RenderTargetView*) * SAVED_RTV_COUNTS);
	ZeroMemory(&m_savedVPs, sizeof(D3D11_VIEWPORT) * m_savedVPCount);
	m_depthOnlyObjects.reserve(50);


	DepthOnlyConstant temp;
	ZeroMemory(&temp, sizeof(DepthOnlyConstant));
	Utility::DXResource::CreateConstantBuffer(temp, pDevice, m_depthOnlyCB);
}

const std::unordered_set<const IDepthRenderable*>& DepthOnlyResources::GetDepthRenderableObjects()
{
	return m_depthOnlyObjects;
}

// To RenderPass class
void DepthOnlyResources::BeginDepthOnlyPass(ID3D11DeviceContext1* pContext)
{
	pContext->OMGetRenderTargets(SAVED_RTV_COUNTS, m_savedRTVs, &m_savedDSV);
	pContext->RSGetViewports(&m_savedVPCount, m_savedVPs);
}

// TODO :: 복구제대로안됨
// RenderPassClass...
void DepthOnlyResources::EndDepthOnlyPass(ID3D11DeviceContext1* pContext)
{
	assert(m_savedDSV != nullptr);

	pContext->OMSetRenderTargets(SAVED_RTV_COUNTS, m_savedRTVs, m_savedDSV);
	pContext->RSSetViewports(m_savedVPCount, m_savedVPs);

	DepthOnlyResources::m_savedDSV = nullptr;
	ZeroMemory(&m_savedRTVs, sizeof(ID3D11RenderTargetView*) * SAVED_RTV_COUNTS);
	ZeroMemory(&m_savedVPs, sizeof(D3D11_VIEWPORT) * m_savedVPCount);
}

Microsoft::WRL::ComPtr<ID3D11Buffer> DepthOnlyResources::GetDepthOnlyCB()
{
	return m_depthOnlyCB;
}

void DepthOnlyResources::OnRegisterDepthOnlyObject(const IDepthRenderable* pObj)
{
	m_depthOnlyObjects.insert(pObj);
}

void DepthOnlyResources::OnDestoryDepthOnlyObject(const IDepthRenderable* pObj)
{
	// 나중에..
	m_depthOnlyObjects.erase(pObj);
}