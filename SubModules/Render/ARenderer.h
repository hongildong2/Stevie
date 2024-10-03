#pragma once
#include "pch.h"

class IRenderProxy
{
public:
	// register to renderer's correct list
	virtual BOOL Initialize(ARenderer* pRenderer) = 0;
	virtual void Destroy() = 0;
};


// proxy objects, if special action is required other than basic rendering, make another proxy object
class RTexture2D : IRenderProxy
{
	unsigned int width;
	unsigned int height;
};

class RBuffer : IRenderProxy
{
	unsigned int size;
	unsigned int count;

};

class RTextureVolume : IRenderProxy
{
	unsigned int width;
	unsigned int height;
};

class RMeshGeometry : IRenderProxy
{

};

class RMaterial : IRenderProxy
{

};
class RMeshComponent : IRenderProxy
{
	RMeshGeometry* mesh;
	RMaterial* material;
};




class Ocean : AObject
{
public:
	RTexture2D* t1;
	RTexture2D* t2;
	RTexture2D* t3;
	RBuffer* b;

	int param1;
	RMeshComponent* meshComp;

	void Update()
	{
		param1 = 1;
		// renderer-> update(buffer)

	}
};

class Cloud : IRenderProxy
{
	// same as Cloud
};

constexpr unsigned int MAX = 1000;

class ARenderer
{
	ARenderer(const ARenderer& other) = delete;
	ARenderer operator=(const ARenderer& other) = delete;

public:
	virtual BOOL Initialize(HWND hWnd, BOOL bEnableDebugLayer, BOOL bEnableGBV, const WCHAR* wchShaderPath) = 0;
	virtual void BeginComptue() = 0;
	virtual void Compute() = 0;
	virtual void EndCompute() = 0;

	virtual void BeginRender() = 0;
	virtual void Render() = 0;
	virtual void EndRender() = 0;
	virtual void Present() = 0;

	virtual BOOL UpdateWindowSize(DWORD dwBackBufferWidth, DWORD dwBackBufferHeight) = 0;

	virtual RTexture2D* CreateTextureFromFile(const WCHAR* wchFileName) = 0;
	virtual RTextureVolume* CreateTextureVolume() = 0;
	virtual RTexture2D* CreateTexture2D() = 0;




	// Renders
	virtual void RenderOpaque(RMeshComponent* pMesh) = 0;
	virtual void RenderTransparent(RMeshComponent* pMesh) = 0;
	virtual void RenderDepthOnly(RMeshComponent* pMesh) = 0;
	// virtual void RenderVolume(RMeshComponent* pMesh) = 0; material will decide whether it is an volume material
	virtual void Copy() = 0;

	// Computes
	virtual void ComputeOcean(Ocean* ocean) = 0;
	virtual void ComputeCloud(Cloud* cloud) = 0;


private:
	RMeshComponent* m_opaques[MAX];
	RMeshComponent* m_transparents[MAX];
	// how to manage computables?

};