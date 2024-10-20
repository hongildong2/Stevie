#pragma once
#include "pch.h"

class MeshComponent final
{
public:
	MeshComponent() = default;
	virtual ~MeshComponent();

	void Initialize(IRenderer* pRenderer, RMeshGeometry* pMeshGeometry, RMaterial* pMaterial);

	void Render(DirectX::SimpleMath::Matrix parentTransform);

	inline const RMeshGeometry* GetMeshGeometry() const
	{
		return m_pMeshGeometry;
	}

	inline const RMaterial* GetMaterial() const
	{
		return m_pMaterial;
	}



private:
	IRenderer* m_pRenderer;
	const RMeshGeometry* m_pMeshGeometry;
	const RMaterial* m_pMaterial;
};

