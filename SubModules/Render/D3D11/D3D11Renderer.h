#pragma once
#include "pch.h"
#include "D3D11ResourceManager.h"
#include "D3D11DeviceResources.h"
#include "D3D11PostProcess.h"

class RMaterial;
class SSceneObject;

class D3D11Renderer
{
public:
	D3D11Renderer() = default;
	~D3D11Renderer() = default;

protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_globalCB;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_sunLightCB;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_meshCB;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_materialCB;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_computeCB;
};