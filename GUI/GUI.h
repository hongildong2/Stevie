#pragma once
#include "pch.h"
#include "Light.h"
#include "Ocean.h"
#include "Model.h"
#include "MeshPart.h"
#include "IComponentHandler.h"
#include "IGUIComponent.h"

class GUI final : public IComponentHandler
{
public:
	GUI(); // TODO :: reserve STL
	void Register(IComponent* comp) override;
	void UnRegister(IComponent* comp) override;
	void Update(); // TODO :: Iterate object, call services using bit flags
	bool Initialize(ID3D11Device1* pDevice, ID3D11DeviceContext1* pContext, HWND window, int width, int height); // TODO :: Initialize ImGui Here
	void Render(); // TODO :: Render ImGui

private:

	std::unordered_set<IGUIComponent*> m_GUIComponents;
};

