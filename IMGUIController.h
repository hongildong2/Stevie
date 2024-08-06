#pragma once
#include "pch.h"
#include "GUI/DTOs.h"

class Model;
class Light;
class Ocean;

class IMGUIController final
{
public:
	IMGUIController() = default;
	~IMGUIController() = default;

	IMGUIController(const IMGUIController& other) = delete;
	IMGUIController& operator=(const IMGUIController& other) = delete;

	void UpdateBegin();
	void UpdateEnd();
	bool Initialize(ID3D11Device1* pDevice, ID3D11DeviceContext1* pContext, HWND window, int width, int height);
	void Render();

	bool UpdateModel(const Model* pModel);
	bool UpdateLight(const Light* pLight);
	bool UpdateOcean(const Ocean* pOcean);

private:
	void drawPos(PosWorldDTO* pInOutPositionDTO);
	void drawRotation(RotationWorldDTO* pInOutRotationDTO);
	void drawLight(LightDTO* pInOutLightDTO);
	void drawOcean(OceanDTO* pInOutOceanDTO);
	void drawMaterial(MaterialDTO* pInOutMaterialDTO);
	void drawPostProcess(PostProcessDTO* pInOutPostProcessDTO);
	// backend instance
};

