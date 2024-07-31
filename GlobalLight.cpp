#include "pch.h"

#include "Utility.h"
#include "GlobalLight.h"
#include "DirectXMath.h"

SceneLights::SceneLights(float shadowMapSize, float nearZ, float farZ)
	: m_shadowViewport{ NULL, }
	, m_nearZ()
	, m_farZ()
{
	m_shadowViewport.TopLeftX = 0;
	m_shadowViewport.TopLeftY = 0;
	m_shadowViewport.Width = float(shadowMapSize);
	m_shadowViewport.Height = float(shadowMapSize);
	m_shadowViewport.MinDepth = 0.0f;
	m_shadowViewport.MaxDepth = 1.0f;
}
void SceneLights::AddLight(const LightData& lightData)
{
	if (m_lightsSB != nullptr) // buffer already initialized, sorry
	{
		return;
	}

	m_lights.push_back(lightData);
}

void SceneLights::Initialize(ID3D11Device1* pDevice)
{
	Utility::DXResource::CreateStructuredBuffer(pDevice, sizeof(LightData), static_cast<UINT>(m_lights.size()), m_lights.data(), m_lightsSB.GetAddressOf());
	Utility::DXResource::CreateBufferSRV(pDevice, m_lightsSB.Get(), m_lightsSRV.GetAddressOf());

	const UINT LIGHTS_COUNT = GetLightsCount();

	// TODO : RenderResource -> DepthMap, ShadowMaps
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	// ����Ʈ�� ������ �³�?
	desc.Width = m_shadowViewport.Width;
	desc.Height = m_shadowViewport.Height;
	desc.MipLevels = 1;
	desc.ArraySize = LIGHTS_COUNT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Format = DXGI_FORMAT_R32_TYPELESS;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	DX::ThrowIfFailed(pDevice->CreateTexture2D(&desc, NULL, m_shadowMaps.GetAddressOf()));


	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY; // �̰� ��� �Ǵ°���??

	dsvDesc.Texture2DArray.MipSlice = 0;
	dsvDesc.Flags = 0;
	for (unsigned int lightIndex = 0; lightIndex < LIGHTS_COUNT; ++lightIndex)
	{
		dsvDesc.Texture2DArray.ArraySize = LIGHTS_COUNT - lightIndex;
		dsvDesc.Texture2DArray.FirstArraySlice = lightIndex;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadowMapDSV;
		DX::ThrowIfFailed(pDevice->CreateDepthStencilView(m_shadowMaps.Get(), &dsvDesc, shadowMapDSV.GetAddressOf()));

		m_shadowMapsDSVs.push_back(shadowMapDSV);
	}


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.ArraySize = LIGHTS_COUNT;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	/*
	* ArraySize�� �ٲ��־���Ѵ�.
	* ���̴��� Texture2D�� ������ �ڵ����� ù��° �ؽ��İ� ���ε��ǳ�?
	* �Ǵ� Texture2DArray�� ���� �� dsvó�� tex[0]���� ����ϳ�
	*/
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	DX::ThrowIfFailed(pDevice->CreateShaderResourceView(m_shadowMaps.Get(), &srvDesc, m_shadowMapsSRV.GetAddressOf()));
}


void SceneLights::Update(ID3D11DeviceContext1* pContext)
{
	const float ORTHO_SIZE = 10.f; // defined in world space unit
	const float FOV_IN_ANGLE = 105.f;

	for (auto& l : m_lights)
	{
		// TODO : ��������� �������ϴ� �� ������Ʈ���� ���� Ŀ���� �� �ֵ��� Directional light�� position�� camera�� position�� ���� ����
		auto view = DirectX::SimpleMath::Matrix::CreateLookAt(l.positionWorld, l.positionWorld + l.direction, { 0.f, 1.f, 0.f });

		// different with viewport, proj matrix coord unit is defined in world space
		auto proj = l.type == ELightType::DIRECTIONAL ?
					DirectX::SimpleMath::Matrix::CreateOrthographic(10.f, 10.f, m_nearZ, m_farZ)
					:
					DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(DirectX::XMConvertToRadians(FOV_IN_ANGLE), 1, m_nearZ, m_farZ);
	
		l.proj = proj;
		l.viewProj = view * proj;
		l.invProj = proj.Invert();

		// for shader
		l.proj = l.proj.Transpose();
		l.viewProj = l.viewProj.Transpose();
		l.invProj = l.invProj.Transpose();
	}
	Utility::DXResource::UpdateBuffer(pContext, m_lightsSB.Get(), sizeof(LightData), static_cast<UINT>(m_lights.size()), m_lights.data());
}