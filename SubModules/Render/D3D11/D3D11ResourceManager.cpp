#include "pch.h"
#include "SubModules/Render/RResourceManager.h"
#include "SubModules/Render/RBuffer.h"
#include "SubModules/Render/RMeshGeometry.h"
#include "SubModules/Render/RTexture.h"
#include "D3D11DeviceResources.h"

RResourceManager::~RResourceManager()
{
	Graphics::ClearCommonResources();
}

void RResourceManager::Initialize(RRenderer* pRenderer)
{
	m_pRenderer = pRenderer;

	InitializeCommonResource();
}

void RResourceManager::CreateVertexBuffer(RBuffer* pOutBuffer, const void* pInVertexList, const UINT vertexSize, const UINT vertexCount)
{
	auto* pDevice = m_pRenderer->GetDeviceResources()->GetD3DDevice();
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = vertexSize * vertexCount;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = pInVertexList;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	ThrowIfFailed(pDevice->CreateBuffer(&bufferDesc, &InitData, pOutBuffer->ReleaseAndGetAddressOf()));



}

void RResourceManager::CreateIndexBuffer(RBuffer* pOutBuffer, const void* pInIndexList, const UINT indexSize, const UINT indexCount)
{
	auto* pDevice = m_pRenderer->GetDeviceResources()->GetD3DDevice();

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = indexSize * indexCount;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = pInIndexList;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	ThrowIfFailed(pDevice->CreateBuffer(&bufferDesc, &InitData, pOutBuffer->ReleaseAndGetAddressOf()));
}

RMeshGeometry* RResourceManager::CreateMeshGeometry(const void* pInVertexList, const UINT vertexSize, const UINT vertexCount, const void* pInIndexList, const UINT indexSize, const UINT indexCount, const EPrimitiveTopologyType topologyType, const EMeshType meshType)
{
	RMeshGeometry* res = new RMeshGeometry(topologyType, meshType);
	res->Initialize(m_pRenderer, pInVertexList, vertexSize, vertexCount, pInIndexList, indexSize, indexCount);
	return res;
}

RTexture* RResourceManager::CreateTexture2D(const UINT width, const UINT height, const UINT count, const DXGI_FORMAT format)
{
	auto* pDevice = m_pRenderer->GetDeviceResources()->GetD3DDevice();
	ComPtr<ID3D11Texture2D> lBuffer;
	ComPtr<ID3D11ShaderResourceView> lSRV;
	ComPtr<ID3D11UnorderedAccessView> lUAV;


	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Format = format;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = count;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = 0;


	ThrowIfFailed(pDevice->CreateTexture2D(&desc, nullptr, lBuffer.ReleaseAndGetAddressOf()));

	CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(D3D11_SRV_DIMENSION_TEXTURE2D, format);
	if (desc.ArraySize > 1)
	{
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		shaderResourceViewDesc.Texture2DArray.ArraySize = count;
		shaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
		shaderResourceViewDesc.Texture2DArray.MipLevels = 1;
		shaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
	}

	ThrowIfFailed(pDevice->CreateShaderResourceView(
		lBuffer.Get(),
		&shaderResourceViewDesc,
		lSRV.ReleaseAndGetAddressOf()
	));


	CD3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc(D3D11_UAV_DIMENSION_TEXTURE2D, format);
	if (desc.ArraySize > 1)
	{
		unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
		unorderedAccessViewDesc.Texture2DArray.ArraySize = count;
		unorderedAccessViewDesc.Texture2DArray.FirstArraySlice = 0;
		unorderedAccessViewDesc.Texture2DArray.MipSlice = 0;
	}
	ThrowIfFailed(pDevice->CreateUnorderedAccessView(lBuffer.Get(), &unorderedAccessViewDesc, lUAV.ReleaseAndGetAddressOf()));

	ETextureType type = count > 1 ? ETextureType::TEXTURE_2D_ARRAY : ETextureType::TEXTURE_2D;
	RTexture* res = new RTexture(type, format, TRUE);
	res->m_resource.Swap(lBuffer);
	res->m_SRV.Swap(lSRV);
	res->m_UAV.Swap(lUAV);
	res->SetSize(width, height, 1, count);
	res->Initialize();

	return res;
}

RTexture* RResourceManager::CreateTexture3D(const UINT width, const UINT height, const UINT depth, const DXGI_FORMAT format)
{
	auto* pDevice = m_pRenderer->GetDeviceResources()->GetD3DDevice();
	ComPtr<ID3D11Texture3D> lBuffer;
	ComPtr<ID3D11ShaderResourceView> lSRV;
	ComPtr<ID3D11UnorderedAccessView> lUAV;


	D3D11_TEXTURE3D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE3D_DESC));
	desc.Format = format;
	desc.Width = width;
	desc.Height = height;
	desc.Depth = depth;
	desc.MipLevels = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = 0;


	ThrowIfFailed(pDevice->CreateTexture3D(&desc, nullptr, lBuffer.ReleaseAndGetAddressOf()));

	CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(D3D11_SRV_DIMENSION_TEXTURE3D, format);

	ThrowIfFailed(pDevice->CreateShaderResourceView(
		lBuffer.Get(),
		&shaderResourceViewDesc,
		lSRV.ReleaseAndGetAddressOf()
	));


	CD3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc(D3D11_UAV_DIMENSION_TEXTURE3D, format);
	ThrowIfFailed(pDevice->CreateUnorderedAccessView(lBuffer.Get(), &unorderedAccessViewDesc, lUAV.ReleaseAndGetAddressOf()));

	RTexture* res = new RTexture(ETextureType::TEXTURE_3D, format, TRUE);
	res->m_resource.Swap(lBuffer);
	res->m_SRV.Swap(lSRV);
	res->m_UAV.Swap(lUAV);

	res->SetSize(width, height, depth, 1);
	res->Initialize();

	return res;
}

RTexture* RResourceManager::CreateTextureDepth(const UINT width, const UINT height)
{
	auto* pDevice = m_pRenderer->GetDeviceResources()->GetD3DDevice();
	ComPtr<ID3D11Texture2D> lBuffer;
	ComPtr<ID3D11ShaderResourceView> lSRV;
	ComPtr<ID3D11DepthStencilView> lDSV;

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Format = DXGI_FORMAT_R32_TYPELESS;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = 0;

	ThrowIfFailed(pDevice->CreateTexture2D(&desc, nullptr, lBuffer.ReleaseAndGetAddressOf()));

	CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R32_FLOAT);

	ThrowIfFailed(pDevice->CreateShaderResourceView(
		lBuffer.Get(),
		&shaderResourceViewDesc,
		lSRV.ReleaseAndGetAddressOf()
	));

	CD3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc(D3D11_DSV_DIMENSION_TEXTURE2D, DXGI_FORMAT_D32_FLOAT);
	ThrowIfFailed(pDevice->CreateDepthStencilView(
		lBuffer.Get(),
		&dsvDesc,
		lDSV.ReleaseAndGetAddressOf()
	));

	RTexture* res = new RTexture(ETextureType::TEXTURE_2D_DEPTH, DXGI_FORMAT_R32_TYPELESS, FALSE);
	res->SetSize(width, height, 1, 1);

	res->m_resource.Swap(lBuffer);
	res->m_SRV.Swap(lSRV);
	res->m_DSV.Swap(lDSV);

	res->Initialize();

	return res;
}

RTexture* RResourceManager::CreateTextureRender(const DXGI_FORMAT format, const UINT width, const UINT height)
{
	auto* pDevice = m_pRenderer->GetDeviceResources()->GetD3DDevice();
	ComPtr<ID3D11Texture2D> lBuffer;
	ComPtr<ID3D11ShaderResourceView> lSRV;
	ComPtr<ID3D11UnorderedAccessView> lUAV;
	ComPtr<ID3D11RenderTargetView> lRTV;


	{
		UINT formatSupport = 0;
		if (FAILED(pDevice->CheckFormatSupport(format, &formatSupport)))
		{
			throw std::runtime_error("CheckFormatSupport");
		}

		UINT32 required = D3D11_FORMAT_SUPPORT_TEXTURE2D | D3D11_FORMAT_SUPPORT_RENDER_TARGET;
		if ((formatSupport & required) != required)
		{
			throw std::runtime_error("RenderTexture");
		}
	}

	if (width > UINT32_MAX || height > UINT32_MAX)
	{
		throw std::out_of_range("Invalid width/height");
	}

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Format = format;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = 0;

	ThrowIfFailed(pDevice->CreateTexture2D(
		&desc,
		nullptr,
		lBuffer.ReleaseAndGetAddressOf()
	));

	// Create RTV.
	CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2D, format);

	ThrowIfFailed(pDevice->CreateRenderTargetView(
		lBuffer.Get(),
		&renderTargetViewDesc,
		lRTV.ReleaseAndGetAddressOf()
	));

	// Create SRV.
	CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(D3D11_SRV_DIMENSION_TEXTURE2D, format);

	ThrowIfFailed(pDevice->CreateShaderResourceView(
		lBuffer.Get(),
		&shaderResourceViewDesc,
		lSRV.ReleaseAndGetAddressOf()
	));

	// Create UAV.
	CD3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc(D3D11_UAV_DIMENSION_TEXTURE2D, format);
	ThrowIfFailed(pDevice->CreateUnorderedAccessView(lBuffer.Get(), NULL, lUAV.ReleaseAndGetAddressOf()));

	RTexture* res = new RTexture(ETextureType::TEXTURE_2D_RENDER, format, TRUE);
	res->m_resource.Swap(lBuffer);
	res->SetSize(width, height, 1, 1);
	res->m_SRV.Swap(lSRV);
	res->m_UAV.Swap(lUAV);
	res->m_RTV.Swap(lRTV);

	res->Initialize();

	return res;
}

RTexture* RResourceManager::CreateTexture2DFromWICFile(const WCHAR* fileName)
{
	auto* pDevice = m_pRenderer->GetDeviceResources()->GetD3DDevice();

	ComPtr<ID3D11Texture2D> lTex2D;
	ComPtr<ID3D11Resource> lResource;
	ComPtr<ID3D11ShaderResourceView> lSRV;
	ThrowIfFailed(CreateWICTextureFromFileEx(pDevice, fileName, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_FLAG(false), DirectX::WIC_LOADER_DEFAULT, lResource.ReleaseAndGetAddressOf(), lSRV.ReleaseAndGetAddressOf()));

	RTexture* res = nullptr;

	if (SUCCEEDED(lResource->QueryInterface(lTex2D.ReleaseAndGetAddressOf())))
	{
		D3D11_TEXTURE2D_DESC desc{};
		lTex2D->GetDesc(&desc);

		res = new RTexture(ETextureType::TEXTURE_2D, desc.Format, FALSE);
		res->SetSize(desc.Width, desc.Height, 1, 1);

		res->m_resource.Swap(lTex2D);
		res->m_SRV.Swap(lSRV);

		res->Initialize();

	}
	else
	{
		MY_ASSERT(FALSE); // Not 2D Tex?
	}

	return res;
}

RTexture* RResourceManager::CreateTexture2DFromDDSFile(const WCHAR* fileName)
{
	auto* pDevice = m_pRenderer->GetDeviceResources()->GetD3DDevice();
	ComPtr<ID3D11Texture2D> lTex2D;
	ComPtr<ID3D11Resource> lResource;
	ComPtr<ID3D11ShaderResourceView> lSRV;

	ThrowIfFailed(CreateDDSTextureFromFileEx(pDevice, fileName, 0, D3D11_USAGE_DEFAULT, D3D10_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_FLAG(false), DDS_LOADER_DEFAULT, lResource.ReleaseAndGetAddressOf(), lSRV.ReleaseAndGetAddressOf(), nullptr));

	RTexture* res = nullptr;

	if (SUCCEEDED(lResource->QueryInterface(lTex2D.ReleaseAndGetAddressOf())))
	{
		D3D11_TEXTURE2D_DESC desc{};
		lTex2D->GetDesc(&desc);


		res = new RTexture(ETextureType::TEXTURE_2D, desc.Format, FALSE);
		res->SetSize(desc.Width, desc.Height, 1, 1);

		res->m_resource.Swap(lTex2D);
		res->m_SRV.Swap(lSRV);

		res->Initialize();
	}
	else
	{
		MY_ASSERT(FALSE); // Not 2D Tex?
	}
	return res;
}

RTexture* RResourceManager::CreateTextureCubeFromDDSFile(const WCHAR* fileName)
{
	auto* pDevice = m_pRenderer->GetDeviceResources()->GetD3DDevice();
	RTexture* res = new RTexture(ETextureType::TEXTURE_CUBE, DXGI_FORMAT_UNKNOWN, FALSE);

	ThrowIfFailed(CreateDDSTextureFromFileEx(pDevice, fileName, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D10_RESOURCE_MISC_FLAG(false), DDS_LOADER_DEFAULT, res->m_resource.ReleaseAndGetAddressOf(), res->m_SRV.GetAddressOf(), nullptr));

	res->SetSize(1, 1, 1, 1);
	res->Initialize();

	return res;
}

RMeshGeometry* RResourceManager::CreateCube()
{
	MeshData md = geometryGenerator::MakeBox(1.f);
	return CreateMeshGeometry(md.verticies.data(), sizeof(RVertex), static_cast<UINT>(md.verticies.size()), md.indicies.data(), sizeof(UINT), static_cast<UINT>(md.indicies.size()), EPrimitiveTopologyType::TRIANGLE_LIST, EMeshType::BASIC);
}

RMeshGeometry* RResourceManager::CreateSphere()
{
	MeshData md = geometryGenerator::MakeSphere(1.f, 30, 30);

	return CreateMeshGeometry(md.verticies.data(), sizeof(RVertex), static_cast<UINT>(md.verticies.size()), md.indicies.data(), sizeof(UINT), static_cast<UINT>(md.indicies.size()), EPrimitiveTopologyType::TRIANGLE_LIST, EMeshType::BASIC);
}

RMeshGeometry* RResourceManager::CreateQuad()
{
	MeshData md = geometryGenerator::MakeSquare(1.f);
	return CreateMeshGeometry(md.verticies.data(), sizeof(RVertex), static_cast<UINT>(md.verticies.size()), md.indicies.data(), sizeof(UINT), static_cast<UINT>(md.indicies.size()), EPrimitiveTopologyType::TRIANGLE_LIST, EMeshType::BASIC);
}

RMeshGeometry* RResourceManager::CreateTessellatedQuad()
{
	MeshData md;
	geometryGenerator::MakeCWQuadPatches(128, &md);


	return CreateMeshGeometry(md.verticies.data(), sizeof(RVertex), static_cast<UINT>(md.verticies.size()), md.indicies.data(), sizeof(UINT), static_cast<UINT>(md.indicies.size()), EPrimitiveTopologyType::QUAD_PATCH, EMeshType::TESSELLATED_QUAD);
}

void RResourceManager::CreateConstantBuffer(const UINT bufferSize, const void* pInitData, RBuffer* pOutBuffer)
{
	auto* pDevice = m_pRenderer->GetDeviceResources()->GetD3DDevice();

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ByteWidth = bufferSize;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	if (pInitData == nullptr)
	{
		ThrowIfFailed(pDevice->CreateBuffer(&desc, NULL, pOutBuffer->ReleaseAndGetAddressOf()));
	}
	else
	{
		D3D11_SUBRESOURCE_DATA initDataDesc;
		ZeroMemory(&initDataDesc, sizeof(initDataDesc));
		initDataDesc.pSysMem = pInitData;
		initDataDesc.SysMemPitch = 0;
		initDataDesc.SysMemSlicePitch = 0;

		ThrowIfFailed(pDevice->CreateBuffer(&desc, &initDataDesc, pOutBuffer->ReleaseAndGetAddressOf()));
	}

}
void RResourceManager::UpdateConstantBuffer(const UINT bufferSize, const void* pData, const RBuffer* pInBuffer)
{
	auto* pContext = m_pRenderer->GetDeviceResources()->GetD3DDeviceContext();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	ThrowIfFailed(pContext->Map(pInBuffer->Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

	memcpy(mappedResource.pData, pData, bufferSize);

	pContext->Unmap(pInBuffer->Get(), 0);
}

RTexture* RResourceManager::CreateStructuredBuffer(const UINT uElementSize, const UINT uElementCount, const void* pInitData)
{

	ComPtr<ID3D11Buffer> lBuffer;
	ComPtr<ID3D11ShaderResourceView> lSRV;
	auto* pDevice = m_pRenderer->GetDeviceResources()->GetD3DDevice();

	D3D11_BUFFER_DESC sbDesc;
	ZeroMemory(&sbDesc, sizeof(sbDesc));
	sbDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	sbDesc.ByteWidth = uElementSize * uElementCount;
	sbDesc.Usage = D3D11_USAGE_DYNAMIC;
	sbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	sbDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	sbDesc.StructureByteStride = uElementSize;

	if (pInitData == nullptr)
	{
		ThrowIfFailed(pDevice->CreateBuffer(&sbDesc, nullptr, lBuffer.ReleaseAndGetAddressOf()));
	}
	else
	{
		D3D11_SUBRESOURCE_DATA initData;
		ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
		initData.pSysMem = pInitData;
		ThrowIfFailed(pDevice->CreateBuffer(&sbDesc, &initData, lBuffer.ReleaseAndGetAddressOf()));
	}


	D3D11_BUFFER_DESC descBuf = {};
	(lBuffer.Get())->GetDesc(&descBuf);


	D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	desc.BufferEx.FirstElement = 0;
	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		// This is a Raw Buffer

		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		desc.BufferEx.NumElements = descBuf.ByteWidth / 4;
	}
	else
	{
		if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
		{
			// This is a Structured Buffer
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.BufferEx.NumElements = uElementCount - 0;
		}
		else
		{
			ThrowIfFailed(E_INVALIDARG);
		}
	}

	ThrowIfFailed(pDevice->CreateShaderResourceView(lBuffer.Get(), &desc, lSRV.ReleaseAndGetAddressOf()));

	RTexture* sb = new RTexture(ETextureType::STRUCTURED_BUFFER, DXGI_FORMAT_UNKNOWN, FALSE);

	sb->m_resource.Swap(lBuffer);
	sb->m_SRV.Swap(lSRV);

	sb->SetSize(uElementSize, 1, 1, uElementCount);
	sb->Initialize();

	return sb;
}

void RResourceManager::UpdateStructuredBuffer(const UINT uElementSize, const UINT uElementCount, const void* pData, RTexture* pInBuffer)
{
	auto* pContext = m_pRenderer->GetDeviceResources()->GetD3DDeviceContext();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	ThrowIfFailed(pContext->Map(pInBuffer->Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
	for (UINT i = 0; i < uElementCount; ++i)
	{
		const void* pDataToWrite = static_cast<const char*>(pData) + (i * uElementSize);
		void* pMapped = static_cast<char*>(mappedResource.pData) + (i * uElementSize);

		std::memcpy(pMapped, pDataToWrite, uElementSize);
	}

	pContext->Unmap(pInBuffer->Get(), 0);

}

void RResourceManager::InitializeCommonResource() const
{
	Graphics::InitCommonResources(m_pRenderer);
}
