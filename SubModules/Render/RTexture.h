#pragma once
#include "RenderDefs.h"

enum class ETextureType
{
	TEXTURE_2D,
	TEXTURE_2D_ARRAY,
	TEXTURE_3D,
	TEXTURE_CUBE,
	STRUCTURED_BUFFER
};

// IRenderResource -> TEXTURE_HANDLE, MESH_GEOMETRY_HANDLE

class RTexture
	: public IRenderResource
{
public:
	inline BOOL IsReadOnly() const
	{
		return m_bIsReadOnly;
	}

	inline UINT GetWidth() const
	{
		return m_width;
	}

	inline UINT GetHeight() const
	{
		return m_height;
	}

	inline UINT GetDepth() const
	{
		MY_ASSERT(m_textureType == ETextureType::TEXTURE_3D);
		return m_depth;
	}

	inline ETextureType GetTextureType() const
	{
		return m_textureType;
	}


protected:
	RTexture(ETextureType type);
	virtual ~RTexture() = default;

protected:
	UINT m_width;
	UINT m_height;
	UINT m_depth;



protected:
	DXGI_FORMAT m_format;
	ETextureType m_textureType;
	BOOL m_bIsReadOnly;
	BOOL m_bInitialized;
};


#ifdef API_D3D11
#define RHIDepthTexture ReverseD3D11DepthTexture
#include "D3D11/D3D11Texture.h"
#endif

class RDepthTexture : public RHIDepthTexture
{
public:
	RDepthTexture(IRenderer* pRenderer);
	~RDepthTexture() = default;

	void SetSize(const UINT width, const UINT height);
	void Initialize();
	void Reset();


 private:
	DXGI_FORMAT m_format;
	ETextureType m_type;
	UINT m_width;
	UINT m_height;


 // TEMP, Testing Reverse OOP
#ifdef API_D3D11
	D3D11Renderer* m_pRenderer;
#endif
};

