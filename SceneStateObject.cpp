#include "pch.h"

#include "SceneStateObject.h"
#include "Utility.h"
#include "DirectXTex.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

float SceneStateObject::NEAR_Z = 0.1f;
float SceneStateObject::FAR_Z = 100.f;
float SceneStateObject::FOV = 90.f;
float SceneStateObject::SHADOW_MAP_SIZE = 1024.f;

SceneStateObject::SceneStateObject()
	:m_camera(std::make_unique<Camera>(Vector3(0.f, 0.2f, -5.f), Vector3(0.f, 0.f, 1.f), Vector3::UnitY, NEAR_Z, FAR_Z, FOV))
{
}

void SceneStateObject::Initialize(ID3D11Device1* pDevice)
{
	const DirectX::SimpleMath::Matrix I;
	m_globalConstant = { I, I,I, I ,I, I , {}, 0.f, {}, 4, NEAR_Z, FAR_Z, 0.f };

	Utility::DXResource::CreateConstantBuffer(m_globalConstant, pDevice, m_globalCB);

	using namespace DirectX;
	DX::ThrowIfFailed(CreateDDSTextureFromFileEx(pDevice, L"./Assets/IBL/OVERCAST_SKY/SKYEnvHDR.dds", 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, DDS_LOADER_DEFAULT, nullptr, m_cubemapEnvView.GetAddressOf(), nullptr));
	DX::ThrowIfFailed(CreateDDSTextureFromFileEx(pDevice, L"./Assets/IBL/OVERCAST_SKY/SKYBrdf.dds", 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D10_RESOURCE_MISC_FLAG(false), DDS_LOADER_DEFAULT, nullptr, m_cubemapBRDFView.GetAddressOf(), nullptr));
	DX::ThrowIfFailed(CreateDDSTextureFromFileEx(pDevice, L"./Assets/IBL/OVERCAST_SKY/SKYDiffuseHDR.dds", 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, DDS_LOADER_DEFAULT, nullptr, m_cubemapIrradianceView.GetAddressOf(), nullptr));
	DX::ThrowIfFailed(CreateDDSTextureFromFileEx(pDevice, L"./Assets/IBL/OVERCAST_SKY/SKYSpecularHDR.dds", 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, DDS_LOADER_DEFAULT, nullptr, m_cubemapSpecularView.GetAddressOf(), nullptr));

	// Light
	{
		m_sceneLights = std::make_unique<SceneLights>();

		using DirectX::SimpleMath::Vector3;
		auto sundir = Vector3(0.f, -2.f, -1.f);
		sundir.Normalize();
		std::unique_ptr<Light> l1 = std::make_unique<Light>(ELightType::SPOT, Vector3(0.f, 0.f, -1.f), Vector3(0.f, 1.f, 2.f));
		std::unique_ptr<Light> l2 = std::make_unique<Light>(ELightType::POINT, Vector3(0.f, 0.f, 1.f), Vector3(0.f, 1.f, -2.f));
		std::unique_ptr<Light> l3 = std::make_unique<Light>(ELightType::DIRECTIONAL, sundir, Vector3(0.f, 1.f, 2.f));

		m_sceneLights->AddLight(std::move(l1));
		m_sceneLights->AddLight(std::move(l2));
		m_sceneLights->AddLight(std::move(l3));
		m_sceneLights->Initialize(pDevice);
	}


}
void SceneStateObject::PrepareRender(ID3D11DeviceContext1* pContext)
{
	pContext->VSSetConstantBuffers(0, 1, m_globalCB.GetAddressOf());
	pContext->PSSetConstantBuffers(0, 1, m_globalCB.GetAddressOf());
	pContext->HSSetConstantBuffers(0, 1, m_globalCB.GetAddressOf());
	pContext->DSSetConstantBuffers(0, 1, m_globalCB.GetAddressOf());
	pContext->GSSetConstantBuffers(0, 1, m_globalCB.GetAddressOf());

	ID3D11ShaderResourceView* resources[5] = {
	m_cubemapEnvView.Get(),
	m_cubemapIrradianceView.Get(),
	m_cubemapSpecularView.Get(),
	m_cubemapBRDFView.Get(),
	m_sceneLights->GetLightsSBSRV()
	};

	ID3D11SamplerState* samplers[2] = {
		Graphics::linearWrapSS.Get(),
		Graphics::linearClampSS.Get(),
	};

	// 공용 리소스
	pContext->PSSetShaderResources(0, 5, resources);
	pContext->VSSetShaderResources(4, 1, resources + 4); // scenelight structured buffer to VS
	pContext->PSSetSamplers(0, 2, samplers);
	pContext->VSSetSamplers(0, 2, samplers);

	//cameara, light에서 뎁스맵가져오고 공용으로 설정하기
	ID3D11ShaderResourceView* depthSRV = m_camera->GetDepthMapSRV(); // set to slot 10
	ID3D11ShaderResourceView* const* aa = m_sceneLights->GetShadowMapSRVs(); // slot 11 ~ 11 + LIGHT COUNT
}

void SceneStateObject::Update(ID3D11DeviceContext1* pContext)
{
	// Global Constant
	{
		m_globalConstant.view = m_camera->GetViewRow();
		m_globalConstant.proj = m_camera->GetProjRow();
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

		m_globalConstant.eyeWorld = m_camera->GetEyePos();
		m_globalConstant.eyeDir = m_camera->GetEyeDir();

		// TODO : update time by timer

		m_globalConstant.globalLightsCount = m_sceneLights->GetLightsCount();
		Utility::DXResource::UpdateConstantBuffer(m_globalConstant, pContext, m_globalCB);
	}
	m_sceneLights->Update(pContext);
}

// RenderPass
void SceneStateObject::RenderProcess(ID3D11DeviceContext1* pContext, ID3D11Texture2D* pBufferToProcess, ID3D11ShaderResourceView* pDepthMapSRV, ID3D11RenderTargetView* pRTVToPresent)
{
	m_postProcess->FillTextureToProcess(pContext, pBufferToProcess);

	m_postProcess->ProcessFog(pContext, pDepthMapSRV);
	m_postProcess->ProcessBloom(pContext);

	m_postProcess->Draw(pContext, pRTVToPresent);
}

void SceneStateObject::OnWindowSizeChange(ID3D11Device1* pDevice, D3D11_VIEWPORT vp, DXGI_FORMAT bufferFormat)
{
	RECT SS = { NULL, };
	SS.left = 0;
	SS.top = 0;
	SS.right = vp.Width;
	SS.bottom = vp.Height;

	m_postProcess.reset();
	m_postProcess = std::make_unique<PostProcess>(SS, bufferFormat);
	m_postProcess->Initialize(pDevice);

	m_camera->OnWindowSizeChange(pDevice, vp, bufferFormat);
}