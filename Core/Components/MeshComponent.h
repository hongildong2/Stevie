#pragma once
#include "pch.h"

class RMaterial;
class RBlendState;

class MeshComponent final
{
public:
	MeshComponent();
	virtual ~MeshComponent();

	void Initialize(IRenderer* pRenderer);
	void SetMaterial(RMaterial* pMaterial);
	void SetMeshGeometry(RMeshGeometry* pMeshGeometry);
	void SetTransparency(RBlendState* pBlendState, Vector4& blendFactor);

	virtual void Update();
	void Render(Matrix& parentTransform);

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
		MY_ASSERT(IsTransparent() == TRUE && m_pBlendState != nullptr);
		return m_pBlendState;
	}

	inline const Vector4& GetBlendFactor() const
	{
		MY_ASSERT(IsTransparent() == TRUE && m_pBlendState != nullptr);
		return m_blendFactor;
	}

	inline BOOL IsTransparent() const
	{
		return m_bIsTransparent;
	}

	inline BOOL IsOccluder() const
	{
		return m_bIsOccluder;
	}

	inline BOOL IsActive() const
	{
		return m_bIsActive;
	}



private:
	IRenderer* m_pRenderer;
	RMeshGeometry* m_pMeshGeometry;
	RMaterial* m_pMaterial;

	// This Component's DrawPolicy is decided by its material or meshGeometry, or can be changed.
	BOOL m_bIsTransparent = false;
	BOOL m_bIsOccluder = true;
	BOOL m_bIsActive = true;

	Vector4 m_blendFactor;
	const RBlendState* m_pBlendState;
};

