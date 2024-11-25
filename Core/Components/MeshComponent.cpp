#include "pch.h"
#include "MeshComponent.h"

MeshComponent::MeshComponent()
{
}

MeshComponent::~MeshComponent()
{
	// delete m_pMeshGeometry;
}



void MeshComponent::Initialize(IRenderer* pRenderer)
{
	m_pRenderer = pRenderer;
}

void MeshComponent::SetTransparency(RBlendState* pBlendState, Vector4& blendFactor)
{
	m_pBlendState = pBlendState;
	m_blendFactor = blendFactor;
	m_bIsTransparent = true;

}

void MeshComponent::SetMaterial(RMaterial* pMaterial)
{
	m_pMaterial = pMaterial;
}

void MeshComponent::SetMeshGeometry(RMeshGeometry* pMeshGeometry)
{
	m_pMeshGeometry = pMeshGeometry;
}

void MeshComponent::Update()
{
	m_pMaterial->Update();
}

void MeshComponent::Render(Matrix& parentTransform)
{
	m_pRenderer->Submit(this, parentTransform);
}
