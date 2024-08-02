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
	static const UINT SAVED_RTV_COUNTS = 8;
	extern UINT SavedDSVCount;
	extern UINT SavedVPCount;
	extern ID3D11DepthStencilView* savedDSV;
	extern ID3D11RenderTargetView* savedRTVs[SAVED_RTV_COUNTS];
	extern D3D11_VIEWPORT savedVPs[10];
	extern Microsoft::WRL::ComPtr<ID3D11Buffer> depthOnlyCB;

	void InitDepthOnlyResources(ID3D11Device1* pDevice);
	void BeginDepthOnlyPass(ID3D11DeviceContext1* pContext);
	void EndDepthOnlyPass(ID3D11DeviceContext1* pContext);
	const std::unordered_set<const IDepthRenderable*>& GetDepthRenderableObjects();

	void OnRegisterDepthOnlyObject(const IDepthRenderable* pObj);
	void OnDestoryDepthOnlyObject(const IDepthRenderable* pObj);
}
