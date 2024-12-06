#include "pch.h"
#include "Ocean.h"
#include "SubModules/Render/Materials/ROceanMaterial.h"
#include "Components/MeshComponent.h"

Ocean::Ocean(RRenderer* pRenderer)
	:m_pRenderer(pRenderer)
{
	m_scale = 75.f;
	m_pitchEuler = XM_PIDIV2;

	RMeshGeometry* tQuad = m_pRenderer->CreateBasicMeshGeometry(EBasicMeshGeometry::TESSELLATED_QUAD);
	ROceanMaterial* oceanMat = new ROceanMaterial(m_pRenderer);
	RTexture* OceanFoamTexture = m_pRenderer->CreateTexture2DFromWICFile(L"./Assets/Textures/Ocean/foam.jpg");
	RTexture* OceanSkyTexture = m_pRenderer->CreateTexture2DFromWICFile(L"./Assets/Textures/Ocean/overcast_sky.jpg");

	oceanMat->SetFoamTexture(OceanFoamTexture);
	oceanMat->SetSkyTexture(OceanSkyTexture);
	oceanMat->Initialize();

	MeshComponent* oceanMC = new MeshComponent();
	oceanMC->SetMaterial(oceanMat);
	oceanMC->SetMeshGeometry(tQuad);
	oceanMC->Initialize(m_pRenderer);


	SetMeshComponent(oceanMC);



}
