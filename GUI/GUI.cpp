#include "pch.h"
#include "GUI.h"
#include "AObject.h"

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

GUI::GUI()
	: m_GUIComponents()
{
	m_GUIComponents.reserve(100);
}

void GUI::Register(IComponent* obj)
{
	if ((obj->GetComponentType() & EComponentsFlag::GUI) == EComponentsFlag::GUI)
	{
		m_GUIComponents.insert(static_cast<IGUIComponent*>(obj));
	}
}
void GUI::UnRegister(IComponent* obj)
{
	if ((obj->GetComponentType() & EComponentsFlag::GUI) == EComponentsFlag::GUI)
	{
		m_GUIComponents.erase(static_cast<IGUIComponent*>(obj));
	}
}

bool GUI::Initialize(ID3D11Device1* pDevice, ID3D11DeviceContext1* pContext, HWND window, int width, int height)
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
void GUI::Update()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();
	ImGui::Begin("Scene Control");



	for (auto* guiComponent : m_GUIComponents)
	{
		EGUIType type = guiComponent->GetGUIType();
		const AObject* rmsidRkqclwlakfrhdiawjsgkrpgkfrjftlqkf = guiComponent->GetThis();

		switch (type)
		{
		case EGUIType::MODEL:
			const Model* model = static_cast<const Model*>(rmsidRkqclwlakfrhdiawjsgkrpgkfrjftlqkf);
			// CALL MODEL CONTROLLER
			break;
		case EGUIType::LIGHT:
			// CALL LIGHT CONTROLLER
			const Light* light = static_cast<const Light*>(rmsidRkqclwlakfrhdiawjsgkrpgkfrjftlqkf);
			break;
		case EGUIType::OCEAN:
			// CALL OCEAN CONTROLLER
			const Ocean* ocean = static_cast<const Ocean*>(rmsidRkqclwlakfrhdiawjsgkrpgkfrjftlqkf);
			break;
		default:
			assert(false);
			break;
		}
	}



	ImGui::End();
	ImGui::Render();
}

void GUI::Render()
{
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}