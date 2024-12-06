#pragma once
#include "pch.h"
#include "RRenderer.h"
#include "RResourceManager.h"
#include "Core/Components/MeshComponent.h"
#include "Core/Skybox.h"
#include "Core/Camera.h"
#include "Core/Light.h"



void RRenderer::Render()
{
	// Update Scene Resources
	{
		UpdateGlobalConstant();
		// Light
		m_resourceManager->UpdateStructuredBuffer(sizeof(RLightConstant), static_cast<UINT>(m_sceneLightsIndex), m_sceneLights, m_sceneLightsBuffer);
	}

	RenderSkybox();

	RenderOpaques();

	RenderTransparent();
}


RMeshGeometry* RRenderer::CreateMeshGeometry(const void* pInVertexList, const UINT vertexSize, const UINT vertexCount, const void* pInIndexList, const UINT indexSize, const UINT indexCount, const EPrimitiveTopologyType topologyType, const EMeshType meshType)
{
	MY_ASSERT(pInVertexList != nullptr && pInIndexList != nullptr);
	return m_resourceManager->CreateMeshGeometry(pInVertexList, vertexSize, vertexCount, pInIndexList, indexSize, indexCount, topologyType, meshType);
}


RMeshGeometry* RRenderer::CreateBasicMeshGeometry(EBasicMeshGeometry type)
{
	switch (type)
	{
	case EBasicMeshGeometry::QUAD:
		return m_resourceManager->CreateQuad();
	case EBasicMeshGeometry::CUBE:
		return m_resourceManager->CreateCube();
	case EBasicMeshGeometry::SPHERE:
		return m_resourceManager->CreateSphere();
	case EBasicMeshGeometry::TESSELLATED_QUAD:
		return m_resourceManager->CreateTessellatedQuad();
	default:
		MY_ASSERT(FALSE);
		return nullptr;
	}
}


RTexture* RRenderer::CreateTexture2DFromWICFile(const WCHAR* wchFileName)
{
	MY_ASSERT(wchFileName != nullptr);
	return m_resourceManager->CreateTexture2DFromWICFile(wchFileName);
}

RTexture* RRenderer::CreateTexture2DFromDDSFile(const WCHAR* wchFileName)
{
	MY_ASSERT(wchFileName != nullptr);
	return m_resourceManager->CreateTexture2DFromDDSFile(wchFileName);
}

RTexture* RRenderer::CreateTextureCubeFromDDSFile(const WCHAR* wchFileName)
{
	MY_ASSERT(wchFileName != nullptr);
	return m_resourceManager->CreateTextureCubeFromDDSFile(wchFileName);
}
RTexture* RRenderer::CreateTexture3D(const UINT width, const UINT height, const UINT depth, const DXGI_FORMAT format)
{
	return m_resourceManager->CreateTexture3D(width, height, depth, format);
}

RTexture* RRenderer::CreateTexture2D(const UINT width, const UINT height, const UINT count, const DXGI_FORMAT format)
{
	return m_resourceManager->CreateTexture2D(width, height, count, format);
}

RTexture* RRenderer::CreateStructuredBuffer(const UINT uElementSize, const UINT uElementCount, const void* pInitData)
{
	return m_resourceManager->CreateStructuredBuffer(uElementSize, uElementCount, pInitData);
}

void RRenderer::SetCamera(const Camera* pCamera)
{
	MY_ASSERT(pCamera != nullptr);

	m_camera = pCamera;
}

void RRenderer::SetSkybox(Skybox* pSkybox)
{
	MY_ASSERT(pSkybox != nullptr);
	m_skybox = pSkybox;
}

void RRenderer::SetSunLight(const Light* pLight)
{
	MY_ASSERT(pLight != nullptr);
	m_sunLight = pLight;
}

void RRenderer::SetIBLTextures(const RTexture* pIrradianceMapTexture, const RTexture* pSpecularMapTexture, const RTexture* pBRDFMapTexture)
{
	MY_ASSERT(pIrradianceMapTexture != nullptr && pSpecularMapTexture != nullptr && pBRDFMapTexture != nullptr);
	MY_ASSERT(pIrradianceMapTexture->GetTextureType() == ETextureType::TEXTURE_CUBE);
	MY_ASSERT(pSpecularMapTexture->GetTextureType() == ETextureType::TEXTURE_CUBE);
	MY_ASSERT(pBRDFMapTexture->GetTextureType() == ETextureType::TEXTURE_2D);

	m_irradianceMapTexture = pIrradianceMapTexture;
	m_specularMapTexture = pSpecularMapTexture;
	m_BRDFMapTexture = pBRDFMapTexture;

}

void RRenderer::AddLight(const Light* pLight)
{
	if (m_sceneLightsIndex >= MAX_SCENE_LIGHTS_COUNT)
	{
		return;
	}

	m_sceneLights[m_sceneLightsIndex] = pLight->GetLightConstant();
	if (pLight->IsShadowingLight() && m_shadowingLightsIndex < MAX_SHADOWING_LIGHTS_COUNT)
	{
		m_shadowingLightsIndices[m_shadowingLightsIndex++] = m_sceneLightsIndex;
	}

	++m_sceneLightsIndex;
}

void RRenderer::Submit(const MeshComponent* pInMeshComponent, Matrix worldRow)
{
	if (m_renderItemIndex >= MAX_RENDER_ITEM || false == pInMeshComponent->IsActive())
	{
		return;
	}

	UINT currentIndex = m_renderItemIndex++;
	RenderItem& newRenderItem = m_renderItems[currentIndex];
	newRenderItem.pMeshGeometry = pInMeshComponent->GetMeshGeometry();


	// Shader Bindings
	const RMaterial* pMat = pInMeshComponent->GetMaterial();
	newRenderItem.pPS = pMat->GetShader();
	newRenderItem.geometrySSCount = pMat->GetGeometrySamplerStatesCount();
	newRenderItem.pixelSSCount = pMat->GetPixelSamplerStatesCount();
	newRenderItem.geometryTexCount = pMat->GetGeometryTexturesCount();
	newRenderItem.pixelTexCount = pMat->GetPixelTexturesCount();

	pMat->GetGeometrySamplerStates(newRenderItem.ppGeometrySamplerStates);
	pMat->GetGeometryTextures(newRenderItem.ppGeometryTextures);
	pMat->GetPixelSamplerStates(newRenderItem.ppPixelSamplerStates);
	pMat->GetPixelTextures(newRenderItem.ppPixelTextures);

	newRenderItem.pBlendState = nullptr;
	newRenderItem.bIsTransparent = false;
	newRenderItem.bIsOccluder = pInMeshComponent->IsOccluder();


	// Parameter, Must be 16byte aligned
	{
		BOOL bIsHeightMapped = newRenderItem.geometryTexCount > 0;
		RMeshConstant meshCB(worldRow, bIsHeightMapped);
		MEMCPY_RENDER_PARAM(&newRenderItem.meshParam, &meshCB);
	}

	if (pMat != nullptr)
	{
		pMat->GetMaterialConstant(&newRenderItem.materialParam);
	}

	// Draw Policy
	{
		if (pInMeshComponent->IsTransparent())
		{
			newRenderItem.bIsTransparent = true;
			newRenderItem.pBlendState = pInMeshComponent->GetBlendState();
			newRenderItem.blendFactor = pInMeshComponent->GetBlendFactor();
		}
	}

}