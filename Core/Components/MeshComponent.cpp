#include "pch.h"
#include "MeshComponent.h"

MeshComponent::~MeshComponent()
{
	delete m_pMeshGeometry;
}



void MeshComponent::Initialize(IRenderer* pRenderer, RMeshGeometry* pMeshGeometry, RMaterial* pMaterial)
{
	m_pRenderer = pRenderer;
	m_pMeshGeometry = pMeshGeometry;
	m_pMaterial = pMaterial;
}

void MeshComponent::Render(DirectX::SimpleMath::Matrix parentTransform)
{
	m_pRenderer->Render(this, parentTransform);
}
