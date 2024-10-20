#include "pch.h"

#include "GUI.h"
#include "Core\Light.h"
#include "Core\Ocean\Ocean.h"
#include "Core\Model.h"

#include "SubModules\Render\Core\MyPostProcess.h"
#include "SubModules\Render\Core\Utility.h"

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
		AObject* rmsidRkqclwlakfrhdiawjsgkrpgkfrjftlqkf = guiComponent->GetThis();
		if (rmsidRkqclwlakfrhdiawjsgkrpgkfrjftlqkf->IsInstantiated() == false)
		{
			continue;
		}


		switch (type)
		{
		case EGUIType::MODEL:
			m_IMGUIController->UpdateModel(static_cast<Model*>(rmsidRkqclwlakfrhdiawjsgkrpgkfrjftlqkf));
			break;
		case EGUIType::LIGHT:
			m_IMGUIController->UpdateLight(static_cast<Light*>(rmsidRkqclwlakfrhdiawjsgkrpgkfrjftlqkf));
			break;
		case EGUIType::OCEAN:
			m_IMGUIController->UpdateOcean(static_cast<Ocean*>(rmsidRkqclwlakfrhdiawjsgkrpgkfrjftlqkf));
			break;
		case EGUIType::POST_PROCESS:
			m_IMGUIController->UpdatePostProcess(static_cast<MyPostProcess*>(rmsidRkqclwlakfrhdiawjsgkrpgkfrjftlqkf));
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