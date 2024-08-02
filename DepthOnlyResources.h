#pragma once
#include "pch.h"

struct DepthOnlyConstant
{
	DirectX::SimpleMath::Matrix viewCol;
	DirectX::SimpleMath::Matrix projCol;
};

class IDepthRenderable;

namespace DepthOnlyResources
{
	extern constexpr UINT SAVED_RTV_COUNTS = 8;
	extern UINT SavedDSVCount;
	extern UINT SavedVPCount;
	extern ID3D11DepthStencilView* savedDSV;
	extern ID3D11RenderTargetView* savedRTVs[SAVED_RTV_COUNTS];
	extern D3D11_VIEWPORT savedVPs[10];
	extern Microsoft::WRL::ComPtr<ID3D11Buffer> depthOnlyCB;
	std::unordered_set<const IDepthRenderable*> depthOnlyObjects;

	extern void InitDepthOnlyResources(ID3D11Device* pDevice);
	extern void SaveRSOMStates(ID3D11DeviceContext1* pContext);
	extern void RestoreRSOMStates(ID3D11DeviceContext1* pContext);
	extern const std::unordered_set<const IDepthRenderable*>& GetDepthRenderableObjects();

	extern void OnRegisterDepthOnlyObject(const IDepthRenderable* pObj);
	extern void OnDestoryDepthOnlyObject(const IDepthRenderable* pObj);
}
