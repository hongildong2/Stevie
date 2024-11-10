#include "pch.h"
#include "MeshComponent.h"

MeshComponent::~MeshComponent()
{
	// delete m_pMeshGeometry;
}



void MeshComponent::Initialize(IRenderer* pRenderer, RMeshGeometry* pMeshGeometry, RMaterial* pMaterial)
{
	m_pRenderer = pRenderer;
	m_pMeshGeometry = pMeshGeometry;
	m_pMaterial = pMaterial;
}

void MeshComponent::SetTransparency(RBlendState* pBlendState, DirectX::SimpleMath::Vector4& blendFactor)
{
	m_pBlendState = pBlendState;
	m_blendFactor = blendFactor;
	m_bIsTransparent = true;

}

void MeshComponent::Update()
{
}

void MeshComponent::Render(DirectX::SimpleMath::Matrix parentTransform)
{
	m_pRenderer->Submit(this, parentTransform);
}
