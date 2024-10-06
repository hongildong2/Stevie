#include "pch.h"
#include "D3D11ResourceManager.h"
#include "../GraphicsCommon1.h"

void D3D11ResourceManager::Initialize(ID3D11Device1* pDevice)
{
	m_pDevice = pDevice;
	m_namedResourcesMap.reserve(500);
	m_unnamedResourcesMap.reserve(500);
}

void D3D11ResourceManager::InitializeCommonResource()
{
	Graphics::D3D11::InitCommonResources(m_pDevice);
}
