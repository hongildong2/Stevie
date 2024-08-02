#include "pch.h"
#include "DepthOnlyResources.h"
#include "Utility.h"

namespace DepthOnlyResources
{
	UINT SavedDSVCount;
	UINT SavedVPCount;
	ID3D11DepthStencilView* savedDSV;
	ID3D11RenderTargetView* savedRTVs[SAVED_RTV_COUNTS];
	D3D11_VIEWPORT savedVPs[10];
	Microsoft::WRL::ComPtr<ID3D11Buffer> depthOnlyCB;
	std::unordered_set<const IDepthRenderable*> depthOnlyObjects;

	void InitDepthOnlyResources(ID3D11Device1* pDevice)
	{
		SavedDSVCount = 0;
		SavedVPCount = 0;
		savedDSV = nullptr;
		ZeroMemory(&savedRTVs, sizeof(ID3D11RenderTargetView*) * SAVED_RTV_COUNTS);
		ZeroMemory(&savedVPs, sizeof(D3D11_VIEWPORT) * SavedVPCount);
		SavedVPCount = 0;
		depthOnlyObjects.reserve(50);


		DepthOnlyConstant temp;
		ZeroMemory(&temp, sizeof(DepthOnlyConstant));
		Utility::DXResource::CreateConstantBuffer(temp, pDevice, depthOnlyCB);
	}

	const std::unordered_set<const IDepthRenderable*>& GetDepthRenderableObjects()
	{
		return depthOnlyObjects;
	}

	// To RenderPass class
	void BeginDepthOnlyPass(ID3D11DeviceContext1* pContext)
	{
		pContext->OMGetRenderTargets(DepthOnlyResources::SAVED_RTV_COUNTS, DepthOnlyResources::savedRTVs, &DepthOnlyResources::savedDSV);
		pContext->RSGetViewports(&DepthOnlyResources::SavedVPCount, DepthOnlyResources::savedVPs);
	}

	// TODO :: 복구제대로안됨
	// RenderPassClass...
	void EndDepthOnlyPass(ID3D11DeviceContext1* pContext)
	{
		assert(savedDSV != nullptr);

		pContext->OMSetRenderTargets(DepthOnlyResources::SAVED_RTV_COUNTS, DepthOnlyResources::savedRTVs, DepthOnlyResources::savedDSV);
		pContext->RSSetViewports(DepthOnlyResources::SavedVPCount, DepthOnlyResources::savedVPs);

		// null
		DepthOnlyResources::SavedDSVCount = 0;
		DepthOnlyResources::savedDSV = nullptr;
		ZeroMemory(&DepthOnlyResources::savedRTVs, sizeof(ID3D11RenderTargetView*) * SAVED_RTV_COUNTS);
		ZeroMemory(&DepthOnlyResources::savedVPs, sizeof(D3D11_VIEWPORT) * DepthOnlyResources::SavedVPCount);
		DepthOnlyResources::SavedVPCount = 0;
	}

	void OnRegisterDepthOnlyObject(const IDepthRenderable* pObj)
	{
		depthOnlyObjects.insert(pObj);
	}

	void OnDestoryDepthOnlyObject(const IDepthRenderable* pObj)
	{
		// 나중에..
		depthOnlyObjects.erase(pObj);
	}
}