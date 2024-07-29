#include "pch.h"

#include "Utility.h"
#include "GlobalLight.h"
#include "DirectXMath.h"

GlobalLight::GlobalLight(float shadowMapSize, float nearZ, float farZ)
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
void GlobalLight::AddLight(const LightData& lightData)
{
	if (m_lightsSB != nullptr) // buffer already initialized, sorry
	{
		return;
	}

	m_lights.push_back(lightData);
}

void GlobalLight::Initialize(ID3D11Device1* pDevice)
{
	Utility::DXResource::CreateStructuredBuffer(pDevice, sizeof(LightData), static_cast<UINT>(m_lights.size()), m_lights.data(), m_lightsSB.GetAddressOf());
	Utility::DXResource::CreateBufferSRV(pDevice, m_lightsSB.Get(), m_lightsSRV.GetAddressOf());
}


void GlobalLight::Update(ID3D11DeviceContext1* pContext)
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