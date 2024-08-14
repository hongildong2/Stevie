#pragma once
#include "pch.h"

struct DepthOnlyConstant
{
	DirectX::SimpleMath::Matrix viewCol;
	DirectX::SimpleMath::Matrix projCol;
};

class IDepthRenderable;

class DepthOnlyResources final
{
public:
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetDepthOnlyCB();
	void InitDepthOnlyResources(ID3D11Device1* pDevice);
	void BeginDepthOnlyPass(ID3D11DeviceContext1* pContext);
	void EndDepthOnlyPass(ID3D11DeviceContext1* pContext);
	const std::unordered_set<const IDepthRenderable*>& GetDepthRenderableObjects();

	void OnRegisterDepthOnlyObject(const IDepthRenderable* pObj);
	void OnDestoryDepthOnlyObject(const IDepthRenderable* pObj);

	static DepthOnlyResources* const GetInstance()
	{
		if (mInstance == nullptr)
		{
			mInstance = new DepthOnlyResources();
		}

		return mInstance;
	}

	static void Destroy()
	{
		delete mInstance;
		mInstance = nullptr;
	}

private:
	DepthOnlyResources();
	~DepthOnlyResources() = default;
	static DepthOnlyResources* mInstance;

	enum { SAVED_RTV_COUNTS = 4 };
	ID3D11DepthStencilView* m_savedDSV;
	ID3D11RenderTargetView* m_savedRTVs[SAVED_RTV_COUNTS];

	UINT m_savedVPCount;
	D3D11_VIEWPORT m_savedVPs[10];

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_depthOnlyCB;
	std::unordered_set<const IDepthRenderable*> m_depthOnlyObjects;
};

