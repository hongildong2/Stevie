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
	ImGui::NewFrame();
	ImGui::Begin("Scene Control");

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
	io.DisplaySize = ImVec2(float(width), float(height));
	ImGui::StyleColorsLight();


	// Setup Platform/Renderer backends
	if (!ImGui_ImplDX11_Init(pDevice, pContext))
	{
		return false;
	}

	if (!ImGui_ImplWin32_Init(window))
	{
		return false;
	}

	return true;
}
void IMGUIController::Render()
{
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

bool IMGUIController::UpdateModel(const Model* pModel)
{
	Vector4 pos = pModel->GetWorldPos();
	PosWorldDTO dto;
}

bool IMGUIController::UpdateLight(const Light* pLight)
{

}

bool IMGUIController::UpdateOcean(const Ocean* pOcean)
{

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

		ImGui::SliderFloat("fallOffStart", &(pInOutLightDTO->fallOffStart),0.f, 10.f);
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
		ImGui::SliderFloat("Gravity", &(pInOutOceanDTO->initialWaveConstants.g), 1.f, 20.f);
		ImGui::SliderFloat("Ocean Depth", &(pInOutOceanDTO->initialWaveConstants.depth), 300.f, 2000.f);


		std::array<ocean::InitialSpectrumParameter, ocean::CASCADE_COUNT>& initialSpectrumParameters = pInOutOceanDTO->initialSpectrumParameters;
		std::array<ocean::CombineParameter, ocean::CASCADE_COUNT>& cascadeCombineParameters = pInOutOceanDTO->cascadeCombineParameters;

		for (unsigned int i = 0; i < ocean::CASCADE_COUNT; ++i)
		{
			ocean::InitialSpectrumParameter& initSpectrumParam = initialSpectrumParameters[i];
			ocean::CombineParameter& cascadeCombineParam = cascadeCombineParameters[i];
			if (ImGui::TreeNode("Initial Wave Spectrum Paramters"))
			{
				// ImGui::SliderFloat("Cascade L, Simulation Size", &(initSpectrumParam.L), 3.f, 2048.f);
				ImGui::SliderFloat("Cascade Scale", &(initSpectrumParam.scale), 0.5f, 3.f);
				ImGui::SliderFloat("Wind Direction", &(initSpectrumParam.angle), -180.f, 180.f);

				ImGui::SliderFloat("Spread Blend", &(initSpectrumParam.spreadBlend), 0.f, 1.f);

				ImGui::SliderFloat("Swell contribution", &(initSpectrumParam.swell), 0.01f, 1.f);

				ImGui::SliderFloat("JONSWAP alpha", &(initSpectrumParam.alpha), 0.005f, 0.1f);
				ImGui::SliderFloat("JONSWAP peakOmega, peak wave frequency (Inverse Proportional to WaveLength)", &(initSpectrumParam.L), 0.3f, 3.f);
				ImGui::SliderFloat("JONSWAP Gamma, Peak Enhancemeent", &(initSpectrumParam.gamma), 3.f, 10.f);
				ImGui::SliderFloat("JONSWAP Short waves Fade", &(initSpectrumParam.shortWavesFade), 0.01f, 1.f);

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Wave Cascade Combine Parameters"))
			{
				
				ImGui::SliderFloat("Cascade Weight", &(cascadeCombineParam.weight), 0.f, 0.2f);
				ImGui::SliderFloat("Shore Modulation", &(cascadeCombineParam.weight), 0.f, 2.f);
				ImGui::TreePop();
			}
		}
	

		ImGui::TreePop();
	}
}

void IMGUIController::drawMaterial(MaterialDTO* pInOutMaterialDTO)
{
	if (ImGui::TreeNode("Material Constants"))
	{
		ImGui::SliderFloat3("albedo", &(pInOutMaterialDTO->albedo.x), 0.f, 1.f);
		ImGui::SliderFloat("Metallic Factor", &(pInOutMaterialDTO->metallicFactor), 0.f, 1.f);
		ImGui::SliderFloat("AO Factor", &(pInOutMaterialDTO->aoFactor), 0.f, 1.f);
		ImGui::SliderFloat("Roughness Factor", &(pInOutMaterialDTO->roughnessFactor), 0.f, 1.f);
		ImGui::SliderFloat("t1", &(pInOutMaterialDTO->t1), 0.f, 5.f);

		ImGui::Checkbox("Use Textures", (bool*)(&(pInOutMaterialDTO->bUseTexture)));

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