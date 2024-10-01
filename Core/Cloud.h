#pragma once
#include "pch.h"
#include "Core\Model.h"
#include "SubModules\Render\Core\VolumetricTexture.h"



/*
* Create 3D Volumetric Noise and its lightmap
* - Update lightmap when lightDir(Directional Sun Light) changes
* - Update when lightmap config changes
* Volumetric PS
* - Sample Volumetric Texture using Model Coordinate -> UVW
* - Raymarching within the box
* Done!
*/

class Cloud final : public Model
{
public:
	Cloud(const unsigned int cloudCounts);
	~Cloud() = default;

	virtual void Initialize(ID3D11Device1* pDevice) override;
	void InitializeData(ID3D11DeviceContext1* context);


	virtual void Update(ID3D11DeviceContext1* pContext) override;

private:
	bool mb_initialized;
	VolumetricTexture m_densityTexture;
	VolumetricTexture m_lightingTexture;
};

