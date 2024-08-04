#pragma once
#include "pch.h"
#include "Light.h"
#include "Ocean.h"
#include "Model.h"
#include "MeshPart.h"
#include "IObjectHandler.h"
#include "AGUIComponent.h"

class GUI final : public IObjectHandler
{
public:
	GUI(); // TODO :: reserve STL
	void Register(AObject* obj) override;
	void UnRegister(AObject* obj) override;
	void Update(); // TODO :: Iterate object, call services using bit flags
	void Initialize(); // TODO :: Initialize ImGui Here
	void Render(); // TODO :: Render ImGui

private:

	std::unordered_set<AGUIComponent*> m_GUIComponents;
};

