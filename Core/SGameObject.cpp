#include "pch.h"
#include "SGameObject.h"

SGameObject::SGameObject()
	: m_pMeshComponent()
	, m_pRenderer()
	, m_pitchEuler()
	, m_yawEuler()
	, m_rollEuler()
	, m_worldPos()
{
}

SGameObject::~SGameObject()
{
	delete m_pMeshComponent;
}

void SGameObject::Initialize(IRenderer* pRenderer, MeshComponent* pMeshComponent)
{
	m_pRenderer = pRenderer;
	m_pMeshComponent = pMeshComponent;
}

void SGameObject::Render()
{
	m_pRenderer->Render(m_pMeshComponent);
}

void SGameObject::Update()
{
}


