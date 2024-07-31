#include "pch.h"

#include "SceneStateObject.h"
#include "Utility.h"
#include "Game.h"
#include "DirectXTex.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

SceneStateObject::SceneStateObject()
	:m_camera(std::make_unique<Camera>(Vector3(0.f, 0.2f, -5.f), Vector3(0.f, 0.f, 1.f), Vector3::UnitY))
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
		m_sceneLights = std::make_unique<SceneLights>(SHADOW_MAP_SIZE, NEAR_Z, FAR_Z);
		LightData light1 = { {5.f, 5.f, 5.f}, 0.f, {0.f, 0.f, 1.f}, 20.f, {0.f, 10.f, -5.f}, 6.f, {1.f, 1.f, 1.f}, 0.f,ELightType::DIRECTIONAL, 0.02f, 0.01f, 1.f, I, I };
		LightData light2 = { {5.f, 5.f, 5.f}, 0.f, {0.f, -1.f, 0.f}, 20.f, {0.f, 5.f, 0.f}, 6.f, {1.f, 1.f, 1.f}, 0.f,ELightType::SPOT, 0.04f, 0.01f, 1.f, I, I };
		LightData light3 = { {5.f, 5.f, 5.f}, 0.f, {0.f, 0.f, -1.f}, 20.f, {0.f, 5.f, 3.f}, 6.f, {1.f, 1.f, 1.f}, 0.f, ELightType::POINT, 0.02f, 0.01f, 1.f, I, I };


		m_sceneLights->AddLight(light1);
		m_sceneLights->AddLight(light2);
		m_sceneLights->AddLight(light3);
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

	ID3D11ShaderResourceView* resources[] = {
	m_cubemapEnvView.Get(),
	m_cubemapIrradianceView.Get(),
	m_cubemapSpecularView.Get(),
	m_cubemapBRDFView.Get(),
	m_sceneLights->GetLightSRV()
	};

	ID3D11SamplerState* samplers[] = {
		Graphics::linearWrapSS.Get(),
		Graphics::linearClampSS.Get(),
	};

	// 공용 리소스
	pContext->PSSetShaderResources(0, 5, resources);
	pContext->PSSetSamplers(0, 2, samplers);
	pContext->VSSetSamplers(0, 2, samplers);
}

void SceneStateObject::Update(ID3D11DeviceContext1* pContext)
{
	// Global Constant
	{
		m_globalConstant.view = m_camera->GetViewMatrix();
		m_globalConstant.proj = m_proj;
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

void SceneStateObject::ProcessRender(ID3D11DeviceContext1* pContext, ID3D11Texture2D* pBufferToProcess, ID3D11ShaderResourceView* pDepthMapSRV, ID3D11RenderTargetView* pRTVToPresent)
{
	m_postProcess->FillTextureToProcess(pContext, pBufferToProcess);

	m_postProcess->ProcessFog(pContext, pDepthMapSRV);
	m_postProcess->ProcessBloom(pContext);

	m_postProcess->Draw(pContext, pRTVToPresent);
}

void SceneStateObject::OnWindowSizeChange(ID3D11Device1* pDevice, RECT size, DXGI_FORMAT bufferFormat)
{
	m_proj = Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(FOV), float(size.right) / float(size.bottom), NEAR_Z, FAR_Z);

	m_postProcess.reset();
	m_postProcess = std::make_unique<PostProcess>(size, bufferFormat);
	m_postProcess->Initialize(pDevice);
}