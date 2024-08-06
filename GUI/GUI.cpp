#include "pch.h"
#include "GUI.h"
#include "AObject.h"

GUI::GUI()
	: m_GUIComponents()
	, m_IMGUIController()
{
	m_GUIComponents.reserve(100);
	m_IMGUIController = std::make_unique<IMGUIController>();
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
	return m_IMGUIController->Initialize(pDevice, pContext, window, width, height);
}

void GUI::Update()
{
	m_IMGUIController->UpdateBegin();


	for (auto* guiComponent : m_GUIComponents)
	{
		EGUIType type = guiComponent->GetGUIType();
		const AObject* rmsidRkqclwlakfrhdiawjsgkrpgkfrjftlqkf = guiComponent->GetThis();
		if (rmsidRkqclwlakfrhdiawjsgkrpgkfrjftlqkf->IsInstantiated() == false)
		{
			continue;
		}


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

	m_IMGUIController->UpdateEnd();
}

void GUI::Render()
{
	m_IMGUIController->Render();
}