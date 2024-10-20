#pragma once
#include "pch.h"
#include "IGUIComponent.h"

#include "SubModules\IComponentHandler.h"
#include "SubModules\GUI\IMGUI\IMGUIController.h"

class GUI final : public IComponentHandler
{
public:
	GUI();
	~GUI() = default;

	GUI(const GUI& other) = delete;
	GUI& operator=(const GUI& other) = delete;

	void Register(IComponent* comp) override;
	void UnRegister(IComponent* comp) override;

	void Update();
	bool Initialize(ID3D11Device1* pDevice, ID3D11DeviceContext1* pContext, HWND window, int width, int height);
	void Render();



private:
	std::unordered_set<IGUIComponent*> m_GUIComponents;
	std::unique_ptr<IMGUIController> m_IMGUIController;
};

