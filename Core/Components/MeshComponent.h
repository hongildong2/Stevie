#pragma once
#include "pch.h"

class RMaterial;
class RBlendState;

class MeshComponent final
{
public:
	MeshComponent() = default;
	virtual ~MeshComponent();

	void Initialize(IRenderer* pRenderer, RMeshGeometry* pMeshGeometry, RMaterial* pMaterial);
	void SetTransparency(RBlendState* pBlendState, DirectX::SimpleMath::Vector4& blendFactor);

	virtual void Update();
	void Render(DirectX::SimpleMath::Matrix parentTransform);

	inline const RMeshGeometry* GetMeshGeometry() const
	{
		return m_pMeshGeometry;
	}

	inline const RMaterial* GetMaterial() const
	{
		return m_pMaterial;
	}

	inline const RBlendState* GetBlendState() const
	{
		MY_ASSERT(IsTransparent() == true && m_pBlendState != nullptr);
		return m_pBlendState;
	}

	inline const DirectX::SimpleMath::Vector4& GetBlendFactor() const
	{
		MY_ASSERT(IsTransparent() == true && m_pBlendState != nullptr);
		return m_blendFactor;
	}

	inline bool IsTransparent() const
	{
		return m_bIsTransparent;
	}

	inline bool IsOccluder() const
	{
		return m_bIsOccluder;
	}

	inline bool IsActive() const
	{
		return m_bIsActive;
	}





private:
	IRenderer* m_pRenderer;
	const RMeshGeometry* m_pMeshGeometry;
	const RMaterial* m_pMaterial;

	bool m_bIsTransparent = false;
	bool m_bIsOccluder = true;
	bool m_bIsActive = true; // IComponent

	DirectX::SimpleMath::Vector4 m_blendFactor;
	const RBlendState* m_pBlendState;
};

