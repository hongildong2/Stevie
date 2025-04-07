#pragma once
#include "pch.h"

class RCloudMaterial final :
	public RMaterial
{
public:
	RCloudMaterial(RRenderer* pRenderer);
	~RCloudMaterial();

	virtual void Initialize() override;
	void InitializeData();
	virtual void Update() override;

	virtual void GetMaterialConstant(RenderParam* pOutRenderParam) const override;

private:
	RTexture* m_densityTexture3D;
	RTexture* m_lightingTexture3D;
};

