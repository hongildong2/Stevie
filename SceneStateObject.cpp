#include "pch.h"

#include "SceneStateObject.h"
#include "Utility.h"
#include "DirectXTex.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

float SceneStateObject::NEAR_Z = 0.1f;
float SceneStateObject::FAR_Z = 120.f;
float SceneStateObject::FOV = 90.f;
float SceneStateObject::SHADOW_MAP_SIZE = 2048.f;

SceneStateObject::SceneStateObject()
	: m_camera(std::make_unique<Camera>(Vector3(0.f, 0.2f, -5.f), Vector3(0.f, 0.f, 1.f), Vector3::UnitY, NEAR_Z, FAR_Z, FOV))
	, m_globalConstant()
{
}

void SceneStateObject::Initialize(ID3D11Device1* pDevice)
{
	const DirectX::SimpleMath::Matrix I;
	m_globalConstant = { I, I,I, I ,I, I , {}, 0.f, {}, 4, NEAR_Z, FAR_Z, 0.f };

	Utility::DXResource::CreateConstantBuffer(m_globalConstant, pDevice, m_globalCB);

	using namespace DirectX;
	DX::ThrowIfFailed(CreateDDSTextureFromFileEx(pDevice, L"./Assets/IBL/NIGHT_SKY/skyEnvHDR.dds", 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, DDS_LOADER_DEFAULT, nullptr, m_cubemapEnvView.GetAddressOf(), nullptr));
	DX::ThrowIfFailed(CreateDDSTextureFromFileEx(pDevice, L"./Assets/IBL/NIGHT_SKY/skyBrdf.dds", 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D10_RESOURCE_MISC_FLAG(false), DDS_LOADER_DEFAULT, nullptr, m_cubemapBRDFView.GetAddressOf(), nullptr));
	DX::ThrowIfFailed(CreateDDSTextureFromFileEx(pDevice, L"./Assets/IBL/NIGHT_SKY/skyDiffuseHDR.dds", 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, DDS_LOADER_DEFAULT, nullptr, m_cubemapIrradianceView.GetAddressOf(), nullptr));
	DX::ThrowIfFailed(CreateDDSTextureFromFileEx(pDevice, L"./Assets/IBL/NIGHT_SKY/skySpecularHDR.dds", 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, DDS_LOADER_DEFAULT, nullptr, m_cubemapSpecularView.GetAddressOf(), nullptr));

	// Light
	{
		m_sceneLights = std::make_unique<SceneLights>();

		using DirectX::SimpleMath::Vector3;

		auto sundir = Vector3(-0.595f, -0.115f, -0.795f);
		sundir.Normalize();

		auto spotDir = Vector3(0.3f, -1.f, 0.f);
		spotDir.Normalize();

		auto pointDir = Vector3(0.f, -1.f, 3.f);
		pointDir.Normalize();

		std::unique_ptr<Light> l1 = std::make_unique<Light>("SPOT LIGHT 1", ELightType::SPOT, spotDir, Vector3(2.f, 2.f, 0.f));
		std::unique_ptr<Light> l2 = std::make_unique<Light>("POINT LIGHT 1", ELightType::POINT, pointDir, Vector3(0.f, 7.f, -2.f));

		std::unique_ptr<Light> l3 = std::make_unique<Light>("SHIP SPOT LIGHT 1", ELightType::SPOT, spotDir, Vector3(0.f, 8.f, 0.f));
		std::unique_ptr<Light> l4 = std::make_unique<Light>("SHIP SPOT LIGHT 2", ELightType::SPOT, spotDir, Vector3(0.f, 7.f, 0.f));
		spotDir.x = -spotDir.x;
		std::unique_ptr<Light> l5 = std::make_unique<Light>("SHIP SPOT LIGHT 3", ELightType::SPOT, spotDir, Vector3(0.f, 7.f, 0.f));
		std::unique_ptr<Light> l6 = std::make_unique<Light>("SHIP SPOT LIGHT 4", ELightType::SPOT, spotDir, Vector3(0.f, 7.f, 0.f));

		std::unique_ptr<Light> l7 = std::make_unique<Light>("ISLAND SPOT LIGHT 1", ELightType::SPOT, spotDir, Vector3(2.5f, 7.f, 0.f));
		spotDir.x = -spotDir.x;
		std::unique_ptr<Light> l8 = std::make_unique<Light>("ISLAND SPOT LIGHT 2", ELightType::SPOT, spotDir, Vector3(-1.6f, 7.f, 0.f));

		std::unique_ptr<Light> l9 = std::make_unique<Light>("DIR LIGHT SUN", ELightType::DIRECTIONAL, sundir, Vector3(20.f, 50.f, -50.f));


		m_sceneLights->AddLight(std::move(l1));
		m_sceneLights->AddLight(std::move(l2));
		m_sceneLights->AddLight(std::move(l3));
		m_sceneLights->AddLight(std::move(l4));
		m_sceneLights->AddLight(std::move(l5));
		m_sceneLights->AddLight(std::move(l6));
		m_sceneLights->AddLight(std::move(l7));
		m_sceneLights->AddLight(std::move(l8));
		m_sceneLights->AddLight(std::move(l9));
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

	ID3D11SamplerState* samplers[4] = {
		Graphics::linearWrapSS.Get(),
		Graphics::linearClampSS.Get(),
		Graphics::shadowPointSS.Get(),
		Graphics::shadowCompareSS.Get()
	};

	// 공용 리소스
	pContext->PSSetShaderResources(0, 5, resources);
	pContext->VSSetShaderResources(4, 1, resources + 4); // scenelight structured buffer to VS
	pContext->PSSetSamplers(0, 4, samplers);
	pContext->VSSetSamplers(0, 4, samplers);

	ID3D11ShaderResourceView* depthSRV = m_camera->GetDepthBufferSRV(); // set to slot 10
	ID3D11ShaderResourceView* const* lightsSRVs = m_sceneLights->GetShadowMapSRVs(); // slot 11 ~ 11 + LIGHT COUNT

	pContext->PSSetShaderResources(10, 1, &depthSRV);
	pContext->PSSetShaderResources(11, m_sceneLights->GetLightsCount(), lightsSRVs);
}

void SceneStateObject::Update(ID3D11DeviceContext1* pContext)
{
	// Global Constant
	{
		auto view = m_camera->GetViewRow();
		auto proj = m_camera->GetProjRow();
		auto viewProj = view * proj;

		m_globalConstant.view = view.Transpose();
		m_globalConstant.proj = proj.Transpose();
		m_globalConstant.viewProj = viewProj.Transpose();

		m_globalConstant.invView = view.Invert().Transpose();
		m_globalConstant.invProj = proj.Invert().Transpose();
		m_globalConstant.invViewProj = viewProj.Invert().Transpose();

		m_globalConstant.eyeWorld = m_camera->GetEyePos();
		m_globalConstant.eyeDir = m_camera->GetEyeDir();

		// TODO : update time by timer

		m_globalConstant.globalLightsCount = m_sceneLights->GetLightsCount();

		auto lol = m_globalConstant;
		Utility::DXResource::UpdateConstantBuffer(lol, pContext, m_globalCB);
	}
	m_sceneLights->Update(pContext);
}

// RenderPass
void SceneStateObject::RenderProcess(ID3D11DeviceContext1* pContext, ID3D11Texture2D* pBufferToProcess, ID3D11RenderTargetView* pRTVToPresent)
{
	m_postProcess->FillTextureToProcess(pContext, pBufferToProcess);

	m_postProcess->ProcessFog(pContext, m_camera->GetDepthBufferSRV());
	m_postProcess->ProcessBloom(pContext);

	m_postProcess->Draw(pContext, pRTVToPresent);
}

void SceneStateObject::OnWindowSizeChange(ID3D11Device1* pDevice, D3D11_VIEWPORT vp, DXGI_FORMAT bufferFormat)
{
	RECT SS = { NULL, };
	SS.left = 0;
	SS.top = 0;
	SS.right = static_cast<LONG>(vp.Width);
	SS.bottom = static_cast<LONG>(vp.Height);

	m_postProcess.reset();
	m_postProcess = std::make_unique<MyPostProcess>(SS, bufferFormat);
	m_postProcess->Initialize(pDevice);

	m_camera->OnWindowSizeChange(pDevice, vp, bufferFormat);
}