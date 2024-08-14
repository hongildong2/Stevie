#include "pch.h"
#include "DepthTexture.h"
#include "Utility.h"

void DepthTexture::Initialize(ID3D11Device1* pDevice)
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.Width = m_viewport.Width;
	desc.Height = m_viewport.Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Format = DXGI_FORMAT_R32_TYPELESS;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = 0;
	dsvDesc.Texture2D.MipSlice = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;


	DX::ThrowIfFailed(pDevice->CreateTexture2D(&desc, NULL, m_depthTexture.GetAddressOf()));
	DX::ThrowIfFailed(pDevice->CreateDepthStencilView(m_depthTexture.Get(), &dsvDesc, m_depthDSV.GetAddressOf()));
	DX::ThrowIfFailed(pDevice->CreateShaderResourceView(m_depthTexture.Get(), &srvDesc, m_depthSRV.GetAddressOf()));
}