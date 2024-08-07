#include "pch.h"
#include "IMGUIController.h"

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "Model.h"
#include "Light.h"
#include "Ocean.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

void IMGUIController::UpdateBegin()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("Scene Control	");

	ImGui::Text("Average %.3f ms/frame (%.1f FPS)",
		1000.0f / ImGui::GetIO().Framerate,
		ImGui::GetIO().Framerate);
}

void IMGUIController::UpdateEnd()
{
	ImGui::End();
	ImGui::Render();
}

bool IMGUIController::Initialize(ID3D11Device1* pDevice, ID3D11DeviceContext1* pContext, HWND window, int width, int height)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.Fonts->AddFontDefault();
	io.Fonts->Build();
	io.DisplaySize = ImVec2(float(width), float(height));
	ImGui::StyleColorsLight();



	if (!ImGui_ImplWin32_Init(window))
	{
		return false;
	}

	// Setup Platform/Renderer backends
	if (!ImGui_ImplDX11_Init(pDevice, pContext))
	{
		return false;
	}

	return true;
}
void IMGUIController::Render()
{
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

bool IMGUIController::UpdateModel(Model* pModel)
{

	// Tree 노드 이름이 다달라야한다!! 같으면 다 연결되어버림
	if (ImGui::TreeNode(pModel->GetName()))
	{
		PosWorldDTO posDTO;
		posDTO.pos = pModel->GetWorldPos();
		drawPos(&posDTO);
		pModel->UpdatePosByCoordinate(posDTO.pos);

		auto& m_meshes = pModel->GetMeshes();
		for (size_t i = 0; i < m_meshes.size(); ++i)
		{
			MaterialDTO matDTO = m_meshes[i]->GetMaterialConstant();

			drawMaterial(&matDTO, i);
			m_meshes[i]->UpdateMaterialConstant(matDTO);
		}

		ImGui::TreePop();
	}

	return true;
}

bool IMGUIController::UpdateLight(Light* pLight)
{
	if (ImGui::TreeNode(pLight->GetName()))
	{
		LightData data;
		pLight->GetLightData(&data);

		PosWorldDTO posDTO;
		posDTO.pos = Vector4(data.positionWorld.x, data.positionWorld.y, data.positionWorld.z, 1.f);
		drawPos(&posDTO);

		pLight->UpdatePosWorld({ posDTO.pos.x, posDTO.pos.y, posDTO.pos.z });

		LightDTO lightDTO = data;
		drawLight(&lightDTO);
		pLight->UpdateLightData(lightDTO);

		ImGui::TreePop();
	}

	return true;
}

bool IMGUIController::UpdateOcean(Ocean* pOcean)
{
	if (ImGui::TreeNode(pOcean->GetName()))
	{
		auto& m_meshes = pOcean->GetMeshes();
		for (size_t i = 0; i < m_meshes.size(); ++i)
		{
			MaterialDTO matDTO = m_meshes[i]->GetMaterialConstant();

			drawMaterial(&matDTO, i);
			m_meshes[i]->UpdateMaterialConstant(matDTO);
		}

		OceanDTO oceanDTO;
		ZeroMemory(&oceanDTO, sizeof(OceanDTO));
		oceanDTO.cascadeCombineParameters = pOcean->GetCascadeCombineParameters();
		oceanDTO.initialSpectrumParameters = pOcean->GetInitialSpectrumParameters();
		oceanDTO.oceanConfiguration = pOcean->GetOceanConfiguration();
		oceanDTO.renderingParams = pOcean->GetRenderingParameter();

		drawOcean(&oceanDTO);

		if (oceanDTO.bResetInitialSpectrumData == true)
		{
			pOcean->OnInitialParameterChanged();
		}

		pOcean->UpdateCombineParameter(oceanDTO.cascadeCombineParameters);
		pOcean->UpdateInitialSpectrumParameter(oceanDTO.initialSpectrumParameters);
		pOcean->UpdateOceanConfiguration(oceanDTO.oceanConfiguration);
		pOcean->UpdateRenderingParameter(oceanDTO.renderingParams);


		ImGui::TreePop();
	}

	return true;
}

void IMGUIController::drawPos(PosWorldDTO* pInOutPositionDTO)
{
	ImGui::SliderFloat3("World Position", &(pInOutPositionDTO->pos.x), -50.f, 50.f);
}

void IMGUIController::drawRotation(RotationWorldDTO* pInOutRotationDTO)
{
	ImGui::SliderFloat("Pitch", &(pInOutRotationDTO->pitch), -180.f, 180.f);
	ImGui::SliderFloat("Yaw", &(pInOutRotationDTO->yaw), -180.f, 180.f);

}

void IMGUIController::drawLight(LightDTO* pInOutLightDTO)
{
	if (ImGui::TreeNode("Light Constants"))
	{
		ImGui::SliderFloat3("radiance", &(pInOutLightDTO->radiance.x), 0.f, 5.f);
		ImGui::SliderFloat3("direction", &(pInOutLightDTO->direction.x), -3.f, 3.f);
		pInOutLightDTO->direction.Normalize();

		ImGui::SliderFloat("fallOffStart", &(pInOutLightDTO->fallOffStart), 0.f, 10.f);
		ImGui::SliderFloat("fallOffEnd", &(pInOutLightDTO->fallOffEnd), 10.f, 50.f);
		ImGui::SliderFloat("spotPower", &(pInOutLightDTO->spotPower), 0.f, 10.f);

		ImGui::ColorEdit3("color", &(pInOutLightDTO->color.x));

		ImGui::SliderFloat("Radius", &(pInOutLightDTO->radius), 0.f, 0.2f);
		ImGui::SliderFloat("Halo Radius", &(pInOutLightDTO->haloRadius), 0.f, 0.3f);
		ImGui::SliderFloat("Halo Strength", &(pInOutLightDTO->haloStrength), 0.f, 5.f);

		ImGui::TreePop();
	}
}

void IMGUIController::drawOcean(OceanDTO* pInOutOceanDTO)
{
	if (ImGui::TreeNode("Ocean Wave Initial Parameters"))
	{
		// Ocean Concfig
		ImGui::SliderFloat("Gravity", &(pInOutOceanDTO->oceanConfiguration.g), 1.f, 20.f);
		ImGui::SliderFloat("Ocean Depth", &(pInOutOceanDTO->oceanConfiguration.depth), 300.f, 2000.f);


		std::array<ocean::InitialSpectrumParameter, ocean::CASCADE_COUNT>& initialSpectrumParameters = pInOutOceanDTO->initialSpectrumParameters;
		std::array<ocean::CombineParameter, ocean::CASCADE_COUNT>& cascadeCombineParameters = pInOutOceanDTO->cascadeCombineParameters;

		// InitialSpectrum, Combine Parameter
		char titleBuffer[100] = { NULL, };


		for (unsigned int i = 0; i < ocean::CASCADE_COUNT; ++i)
		{
			ocean::InitialSpectrumParameter& initSpectrumParam = initialSpectrumParameters[i];
			ocean::CombineParameter& cascadeCombineParam = cascadeCombineParameters[i];

			snprintf(titleBuffer, 100, "%s%d", "Inital Wave Spectrum Parameters :: CASCADE : ", i);
			if (ImGui::TreeNode(titleBuffer))
			{
				// ImGui::SliderFloat("Cascade L, Simulation Size", &(initSpectrumParam.L), 3.f, 2048.f);
				ImGui::SliderFloat("Cascade Scale", &(initSpectrumParam.scale), 0.5f, 3.f);
				ImGui::SliderFloat("Wind Direction", &(initSpectrumParam.angle), -180.f, 180.f);

				ImGui::SliderFloat("Spread Blend", &(initSpectrumParam.spreadBlend), 0.f, 1.f);

				ImGui::SliderFloat("Swell contribution", &(initSpectrumParam.swell), 0.01f, 1.f);

				ImGui::SliderFloat("JONSWAP alpha", &(initSpectrumParam.alpha), 0.005f, 0.1f);
				ImGui::SliderFloat("JONSWAP peakOmega, peak wave freq", &(initSpectrumParam.peakOmega), 0.01f, 1.f);
				ImGui::SliderFloat("JONSWAP Gamma, Peak Enhancemeent", &(initSpectrumParam.gamma), 3.f, 20.f);
				ImGui::SliderFloat("JONSWAP Short waves Fade", &(initSpectrumParam.shortWavesFade), 0.01f, 1.f);

				ImGui::TreePop();
			}

			snprintf(titleBuffer, 100, "%s%d", "Wave Cascade Combine Parameters :: CASCADE : ", i);
			if (ImGui::TreeNode(titleBuffer))
			{

				ImGui::SliderFloat("Cascade Weight", &(cascadeCombineParam.weight), 0.f, 0.2f);
				ImGui::SliderFloat("Shore Modulation", &(cascadeCombineParam.shoreModulation), 0.f, 2.f);
				ImGui::TreePop();
			}
		}
		// 트리 열어서 수정했을때만 재계산
		pInOutOceanDTO->bResetInitialSpectrumData = true;
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Pixel Shader Rendering Parameters"))
	{
		ocean::RenderingParameter& renderParams = pInOutOceanDTO->renderingParams;
		if (ImGui::TreeNode("Subscatter factor, Specular and Multipliers"))
		{
			ImGui::SliderFloat("Horizon Fog Param", &(renderParams.horizonFogParameter), 0.1f, 4.f);
			ImGui::SliderFloat("SSS Normal Strength", &(renderParams.sssNormalStrength), 0.1f, 1.f);
			ImGui::SliderFloat("SSS Ocean Wave Reference Height", &(renderParams.sssOceanWaveReferenceHeight), 1.f, 10.f);
			ImGui::SliderFloat("SSS Wave Height Bias", &(renderParams.sssWaveHeightBias), 1.f, 3.f);
			ImGui::SliderFloat("SSS Sun Strength", &(renderParams.sssSunStrength), 1.f, 10.f);
			ImGui::SliderFloat("SSS Environment Strength", &(renderParams.sssEnvironmentStrength), 1.f, 10.f);
			ImGui::SliderFloat("SSS Spread", &(renderParams.sssSpread), 0.01f, 1.f);
			ImGui::SliderFloat("SSS Fade Distance", &(renderParams.sssFadeDistance), 0.1f, 30.f);


			ImGui::SliderFloat("Direct Light Radiance Scaler", &(renderParams.directLightScaler), 5.f, 50.f);
			ImGui::SliderFloat("Roughness Multiplier", &(renderParams.roughnessMultiplier), 1.f, 30.f);
			ImGui::SliderFloat("Sun Specular Strength", &(renderParams.specularStrength), 1.f, 10.f);
			ImGui::SliderFloat("Shadow Multiplier", &(renderParams.shadowMultiplier), 0.5f, 2.f);
			ImGui::SliderFloat("Mean Fresnel Weight", &(renderParams.meanFresnelWeight), 0.01f, 0.5f);

			ImGui::ColorEdit3("Depth Scatter Color", &(renderParams.depthScatterColor.x));
			ImGui::ColorEdit3("SSS Color", &(renderParams.sssColor.x));
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Wave Normal Variance"))
		{
			ImGui::SliderFloat("Wind Speed", &(renderParams.windSpeed), 1.f, 50.f);
			ImGui::SliderFloat("Wave Alignment", &(renderParams.waveAlignment), 0.1f, 20.f);
			ImGui::SliderFloat("Scale", &(renderParams.scale), 1.f, 4096.f);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Foam"))
		{
			ImGui::SliderFloat("Foam Wave Sharpness", &(renderParams.foamWaveSharpness), 0.01f, 1.f);
			ImGui::SliderFloat("Foam Persistency", &(renderParams.foamPersistency), 0.01f, 1.f);
			ImGui::SliderFloat("Foam Density", &(renderParams.foamDensity), 0.01f, 0.5f);
			ImGui::SliderFloat("Foam Coverage", &(renderParams.foamCoverage), 0.1f, 1.f);
			ImGui::SliderFloat("Foam Trailness", &(renderParams.foamTrailness), 0.1f, 1.f);
			ImGui::SliderFloat("Foam Value Bias", &(renderParams.foamValueBias), 0.01f, 1.f);

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}

void IMGUIController::drawMaterial(MaterialDTO* pInOutMaterialDTO, const unsigned int meshPartIndex)
{
	char buffer[100] = { NULL, };
	snprintf(buffer, 100, "%s%d%s", "Material :: ", meshPartIndex, "Constant");
	if (ImGui::TreeNode(buffer))
	{
		ImGui::ColorEdit3("albedo", &(pInOutMaterialDTO->albedo.x));
		ImGui::SliderFloat("Metallic Factor", &(pInOutMaterialDTO->metallicFactor), 0.f, 1.f);
		ImGui::SliderFloat("AO Factor", &(pInOutMaterialDTO->aoFactor), 0.f, 1.f);
		ImGui::SliderFloat("Roughness Factor", &(pInOutMaterialDTO->roughnessFactor), 0.f, 1.f);
		ImGui::SliderFloat("t1", &(pInOutMaterialDTO->t1), 0.f, 5.f);

		// ImGui::Checkbox("Use Textures", (bool*)(&(pInOutMaterialDTO->bUseTexture)));

		ImGui::SliderFloat("metallic", &(pInOutMaterialDTO->metallic), 0.f, 1.f);
		ImGui::SliderFloat("roughness", &(pInOutMaterialDTO->roughness), 0.f, 1.f);
		ImGui::SliderFloat("specular", &(pInOutMaterialDTO->specular), 0.f, 1.f);

		ImGui::TreePop();
	}
}

void IMGUIController::drawPostProcess(PostProcessDTO* pInOutPostProcessDTO)
{
	// ImGui::SetNextItemOpen(true, ImGuiCond_Once);

	if (ImGui::TreeNode("Post Process Parameters"))
	{
		ImGui::SliderFloat("Bloom Strength", &(pInOutPostProcessDTO->bloomStrength), 0.f, 1.f);
		ImGui::SliderFloat("Exposure", &(pInOutPostProcessDTO->exposure), 0.f, 3.f);
		ImGui::SliderFloat("Gamma", &(pInOutPostProcessDTO->gamma), 0.f, 3.f);
		ImGui::TreePop();
	}

}