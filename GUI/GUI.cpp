#include "pch.h"
#include "GUI.h"

void GUI::Register(AObject* obj)
{
	m_GUIComponents.insert(reinterpret_cast<AGUIComponent*>(obj));
}
void GUI::UnRegister(AObject* obj)
{
	m_GUIComponents.erase(reinterpret_cast<AGUIComponent*>(obj));
}

void GUI::Update()
{

}