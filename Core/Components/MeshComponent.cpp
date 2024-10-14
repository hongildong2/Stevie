#include "pch.h"
#include "MeshComponent.h"

MeshComponent::~MeshComponent()
{
	delete m_pMeshGeometry;
}

void MeshComponent::Initialize(RMeshGeometry* pMeshGeometry, RMaterial* pMaterial)
{
	m_pMeshGeometry = pMeshGeometry;
	m_pMaterial = pMaterial;
}