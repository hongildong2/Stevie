#pragma once
#include "../RConstantBuffer.h"

class D3D11Renderer;

class D3D11ConstantBuffer : public RConstantBuffer
{
public:
	D3D11ConstantBuffer() = default;
	~D3D11ConstantBuffer() = default;

	void Initialize(const D3D11Renderer* pRenderer, const UINT size);
	virtual BOOL UpdateBuffer(const void* pInData, const UINT size) override;
private:
	const D3D11Renderer* pRenderer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_CB;
};

