#include "pch.h"

#include "SceneStateObject.h"
#include "Utility.h"
#include "Game.h"


void SceneStateObject::Initialize(ID3D11Device1* pDevice)
{
	const DirectX::SimpleMath::Matrix I;
	LightData sunLight = { {5.f, 5.f, 5.f}, 0.f, {0.f, -1.f, -0.9f}, 20.f, {0.f, 50.f, 50.f}, 6.f, {1.f,1.f,1.f}, 0.f ,ELightType::SUN, 0.02f, 0.01f, 1.f, I,
	I };
	m_globalConstant = { I, I,I, I ,I, I , {0.f, 0.f, 0.f}, 0.f, 3, NEAR_Z, FAR_Z, 0.f, sunLight };

	Utility::DXResource::CreateConstantBuffer(m_globalConstant, pDevice, m_globalCB);
}
void SceneStateObject::PrepareRender(ID3D11DeviceContext1* pContext)
{
	pContext->VSSetConstantBuffers(0, 1, m_globalCB.GetAddressOf());
	pContext->PSSetConstantBuffers(0, 1, m_globalCB.GetAddressOf());
	pContext->HSSetConstantBuffers(0, 1, m_globalCB.GetAddressOf());
	pContext->DSSetConstantBuffers(0, 1, m_globalCB.GetAddressOf());
	pContext->GSSetConstantBuffers(0, 1, m_globalCB.GetAddressOf());
}

void SceneStateObject::Update(ID3D11DeviceContext1* pContext, Game* pGame)
{
	m_globalConstant.view = pGame->m_camera->GetViewMatrix();
	m_globalConstant.proj = pGame->m_proj;
	m_globalConstant.viewProj = m_globalConstant.view * m_globalConstant.proj;

	m_globalConstant.invView = m_globalConstant.view.Invert();
	m_globalConstant.invProj = m_globalConstant.proj.Invert();
	m_globalConstant.invViewProj = m_globalConstant.viewProj.Invert();

	// Row -> Column wise
	m_globalConstant.view = m_globalConstant.view.Transpose();
	m_globalConstant.proj = m_globalConstant.proj.Transpose();
	m_globalConstant.viewProj = m_globalConstant.viewProj.Transpose();

	m_globalConstant.invView = m_globalConstant.invView.Transpose();
	m_globalConstant.invProj = m_globalConstant.invProj.Transpose();
	m_globalConstant.invViewProj = m_globalConstant.invViewProj.Transpose();

	m_globalConstant.eyeWorld = pGame->m_camera->GetEyePos();

	// update time by timer

	m_globalConstant.globalLightsCount = pGame->m_sceneLights->GetLightsCount();

	auto toSend = m_globalConstant;

	Utility::DXResource::UpdateConstantBuffer(toSend, pContext, m_globalCB);
}