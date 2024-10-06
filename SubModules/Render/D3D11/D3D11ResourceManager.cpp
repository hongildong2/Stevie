#include "pch.h"
#include "D3D11ResourceManager.h"
#include "../GraphicsCommon1.h"

D3D11ResourceManager::~D3D11ResourceManager()
{
	Graphics::D3D11::ClearCommonResources();
}

void D3D11ResourceManager::Initialize(D3D11Renderer* pRenderer)
{
	m_pRenderer = pRenderer;
	m_namedResourcesMap.reserve(500);
	m_unnamedResourcesMap.reserve(500);
}

void D3D11ResourceManager::InitializeCommonResource() const
{
	Graphics::D3D11::InitCommonResources(m_pRenderer);
}
