#include "pch.h"
#include "Cloud.h"
#include "SubModules/Render/Materials/RCloudMaterial.h"
#include "Core/Components/MeshComponent.h"

Cloud::Cloud(IRenderer* pRenderer)
{
	RCloudMaterial* mat = new RCloudMaterial(pRenderer);
	mat->Initialize();

	RMeshGeometry* cube = pRenderer->CreateBasicMeshGeometry(EBasicMeshGeometry::CUBE);

	MeshComponent* pMC = new MeshComponent();
	pMC->SetMaterial(mat);
	pMC->SetMeshGeometry(cube);
	auto aa = Vector4(1.f, 1.f, 1.f, 1.f);
	pMC->SetTransparency(Graphics::ALPHA_BS, aa);

	pMC->Initialize(pRenderer);

	SetMeshComponent(pMC);

	m_scale = 2.f;

	Vector3 d(-5.f, 5.f, 0.f);
	UpdatePos(d);

}
