#pragma once
#include "../IRenderer.h"
#include "D3D11ResourceManager.h"
#include "D3D11DeviceResources.h"


class D3D11Renderer : public IRenderer
{
public:
	D3D11Renderer();
	~D3D11Renderer() = default;
	virtual BOOL Initialize(BOOL bEnableDebugLayer, BOOL bEnableGBV, const WCHAR* wchShaderPath) override;

	virtual void BeginRender() override;
	virtual void EndRender() override;
	virtual void Present() override;

	virtual BOOL SetWindow(HWND hWnd, DWORD dwBackBufferWidth, DWORD dwBackBufferHeight) override;
	virtual BOOL UpdateWindowSize(DWORD dwBackBufferWidth, DWORD dwBackBufferHeight) override;

	// Renders
	virtual void Render(const MeshComponent* pInMeshComponent) override;
	virtual void RenderOcean(const OceanMeshComponent* pInOcean) override;
	virtual void RenderCloud(const CloudMeshComponent* pInRender) override;

	virtual void Compute(const RTexture** pResults, const UINT resultsCount, const RTexture** pResources, const UINT resourcesCount, const RSamplerState** pSamplerStates, const UINT samplerStatesCount, const void** alignedConstants, const UINT** constantSizes, const UINT constantsCount, const UINT batchX, const UINT batchY, const UINT batchZ) override;


	// IRenderDevice
	virtual RTexture2D* CreateTextureFromFile(const WCHAR* wchFileName) override;
	virtual RTexture3D* CreateTexture3D(const UINT width, const UINT height, const UINT depth, const DXGI_FORMAT format) override;
	virtual RTexture2D* CreateDynamicTexture(const UINT width, const UINT height, const DXGI_FORMAT format) override;

	virtual RMeshGeometry* CreateMeshGeometry(const void* pInVertexList, const UINT vertexSize, const UINT vertexCount, const void* pInIndexList, const UINT indexSize, const UINT indexCount) override;
	virtual RMeshGeometry* CreateQuadPatches(const UINT patchCount) override;



	inline const D3D11DeviceResources* GetDeviceResources() const
	{
		return m_deviceResources.get();
	}

	inline const WCHAR* GetShaderPath() const
	{
		return m_shaderPath.c_str();
	}

private:
	std::unique_ptr <D3D11DeviceResources> m_deviceResources;
	std::unique_ptr <D3D11ResourceManager> m_resourceManager;
	std::wstring m_shaderPath;

	// TODO :: maintain several render lists of mesh component according to its type, for multiple pass
	// Material에 따라 RS, BS, PS등등 파이프라인 설정하고 메시 지오메트리를 Draw함으로써 렌더한다.
	// Each Material is matched with specific Pixel Shader. resource difference is handled by flag of binding
	// 1. DepthOnlyPass : Set RS, IL, VS, Draw Opaque mesh
	// 2. Render : Look into Material's resource, and type, set pipeline states like Raster state, Blend state, Pixel shader, resources. Then Draw mesh geometry
	// 3. TransparentPass : Render transparent MeshComponents, need to cast D3D11RenderResources
	// PostProcess : trivial

};