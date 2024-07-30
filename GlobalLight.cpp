#include "pch.h"

#include "Utility.h"
#include "GlobalLight.h"
#include "DirectXMath.h"

SceneLights::SceneLights(float shadowMapSize, float nearZ, float farZ)
	: m_shadowViewport(),
	m_proj()
{
	// Row space matrix
	m_proj = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		DirectX::XMConvertToRadians(90.f),
		float(shadowMapSize) / float(shadowMapSize), nearZ, farZ);

	ZeroMemory(&m_shadowViewport, sizeof(D3D11_VIEWPORT));
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
	// Add sunlight here
	using namespace DirectX::SimpleMath;
	LightData sun = { {5.f, 5.f, 5.f}, 0.f, {0.f, -0.2f, -1.f}, 20.f, {0.f, 0.f, -2.f}, 6.f, {1.f, 1.f, 1.f}, 0.f,ELightType::SUN, 0.02f, 0.01f, 1.f, Matrix(), Matrix() };
	sun.direction.Normalize();
	m_lights.push_back(sun);

	Utility::DXResource::CreateStructuredBuffer(pDevice, sizeof(LightData), static_cast<UINT>(m_lights.size()), m_lights.data(), m_lightsSB.GetAddressOf());
	Utility::DXResource::CreateBufferSRV(pDevice, m_lightsSB.Get(), m_lightsSRV.GetAddressOf());


	// TODO : RenderResource -> DepthMap 클래스 만들고 통일하기
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	// 뷰포트랑 같은것 맞나?
	desc.Width = m_shadowViewport.Width;
	desc.Height = m_shadowViewport.Height;
	desc.MipLevels = 1;
	desc.ArraySize = static_cast<UINT>(m_lights.size());
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
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY; // 이게 어떻게 되는거임?? -> 이건 subresource마다, 라이트마다 DSV를 각각만들어야할듯
	dsvDesc.Texture2DArray.ArraySize = lightCount;
	dsvDesc.Texture2DArray.FirstArraySlice = 몇번째 텍스쳐;
	dsvDesc.Texture2DArray.MipSlice = no mipmapping;
	DX::ThrowIfFailed(pDevice->CreateDepthStencilView(m_shadowMaps.Get(), &dsvDesc, m_shadowMapsDSV.GetAddressOf()));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.ArraySize = lightCount;
	srvDesc.Texture2DArray.FirstArraySlice = 0; // 이걸로 쉐이더에서 Texture2D로 정의하면, 어레이의 subresource처럼 쓸 수 있는듯
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	DX::ThrowIfFailed(pDevice->CreateShaderResourceView(m_shadowMaps.Get(), &srvDesc, m_shadowMapsSRV.GetAddressOf()));
}


void SceneLights::Update(ID3D11DeviceContext1* pContext)
{
	for (auto& l : m_lights)
	{
		auto view = DirectX::SimpleMath::Matrix::CreateLookAt(l.positionWorld, l.direction, { 0.f, 1.f, 0.f });

		l.proj = m_proj;
		l.viewProj = view * m_proj;
		l.invProj = m_proj.Invert();

		// for shader
		l.proj = l.proj.Transpose();
		l.viewProj = l.viewProj.Transpose();
		l.invProj = l.invProj.Transpose();
	}
	Utility::DXResource::UpdateBuffer(pContext, m_lightsSB.Get(), sizeof(LightData), static_cast<UINT>(m_lights.size()), m_lights.data());
}