#include "pch.h"

#include "D3D11Renderer.h"
#include "D3D11Texture.h"
#include "D3D11MeshGeometry.h"
#include "D3D11DeviceResources.h"
#include "D3D11ResourceManager.h"


D3D11ResourceManager::~D3D11ResourceManager()
{
	Graphics::D3D11::ClearCommonResources();
}

void D3D11ResourceManager::Initialize(D3D11Renderer* pRenderer)
{
	m_pRenderer = pRenderer;

	InitializeCommonResource();
}

void D3D11ResourceManager::CreateVertexBuffer(ID3D11Buffer** pOutBuffer, const void* pInVertexList, const UINT vertexSize, const UINT vertexCount)
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
	DX::ThrowIfFailed(pDevice->CreateBuffer(&bufferDesc, &InitData, pOutBuffer));



}

void D3D11ResourceManager::CreateIndexBuffer(ID3D11Buffer** pOutBuffer, const void* pInIndexList, const UINT indexSize, const UINT indexCount)
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
	DX::ThrowIfFailed(pDevice->CreateBuffer(&bufferDesc, &InitData, pOutBuffer));
}

D3D11MeshGeometry* D3D11ResourceManager::CreateMeshGeometry(const void* pInVertexList, const UINT vertexSize, const UINT vertexCount, const void* pInIndexList, const UINT indexSize, const UINT indexCount)
{
	D3D11MeshGeometry* res = new D3D11MeshGeometry(EPrimitiveTopologyType::TRIANGLE_LIST, EMeshType::BASIC);
	res->Initialize(m_pRenderer, pInVertexList, vertexSize, vertexCount, pInIndexList, indexSize, indexCount);
	return res;
}

D3D11Texture2D* D3D11ResourceManager::CreateTexture2D(const UINT width, const UINT height, const UINT count, const DXGI_FORMAT format)
{
	auto* pDevice = m_pRenderer->GetDeviceResources()->GetD3DDevice();

	D3D11Texture2D* res = new D3D11Texture2D();

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


	DX::ThrowIfFailed(pDevice->CreateTexture2D(&desc, nullptr, res->m_resource.ReleaseAndGetAddressOf()));

	CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(D3D11_SRV_DIMENSION_TEXTURE2D, format);
	if (desc.ArraySize > 1)
	{
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		shaderResourceViewDesc.Texture2DArray.ArraySize = count;
		shaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
		shaderResourceViewDesc.Texture2DArray.MipLevels = 1;
		shaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
	}

	DX::ThrowIfFailed(pDevice->CreateShaderResourceView(
		res->m_resource.Get(),
		&shaderResourceViewDesc,
		res->m_SRV.ReleaseAndGetAddressOf()
	));


	CD3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc(D3D11_UAV_DIMENSION_TEXTURE2D, format);
	if (desc.ArraySize > 1)
	{
		unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
		unorderedAccessViewDesc.Texture2DArray.ArraySize = count;
		unorderedAccessViewDesc.Texture2DArray.FirstArraySlice = 0;
		unorderedAccessViewDesc.Texture2DArray.MipSlice = 0;
	}
	DX::ThrowIfFailed(pDevice->CreateUnorderedAccessView(res->m_resource.Get(), NULL, res->m_UAV.ReleaseAndGetAddressOf()));


	return res;
}

D3D11Texture3D* D3D11ResourceManager::CreateTexture3D(const UINT width, const UINT height, const UINT depth, const DXGI_FORMAT format)
{
	D3D11Texture3D* res = new D3D11Texture3D();
	auto* pDevice = m_pRenderer->GetDeviceResources()->GetD3DDevice();

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


	DX::ThrowIfFailed(pDevice->CreateTexture3D(&desc, nullptr, res->m_resource.ReleaseAndGetAddressOf()));

	CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(D3D11_SRV_DIMENSION_TEXTURE3D, format);

	DX::ThrowIfFailed(pDevice->CreateShaderResourceView(
		res->m_resource.Get(),
		&shaderResourceViewDesc,
		res->m_SRV.ReleaseAndGetAddressOf()
	));


	CD3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc(D3D11_UAV_DIMENSION_TEXTURE3D, format);
	DX::ThrowIfFailed(pDevice->CreateUnorderedAccessView(res->m_resource.Get(), &unorderedAccessViewDesc, res->m_UAV.ReleaseAndGetAddressOf()));

	res->Initialize(width, height, depth, format, FALSE);
	return res;
}

D3D11TextureDepth* D3D11ResourceManager::CreateTextureDepth(const UINT width, const UINT height)
{
	D3D11TextureDepth* res = new D3D11TextureDepth();
	auto* pDevice = m_pRenderer->GetDeviceResources()->GetD3DDevice();

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

	DX::ThrowIfFailed(pDevice->CreateTexture2D(&desc, nullptr, res->m_resource.GetAddressOf()));

	CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R32_FLOAT);

	DX::ThrowIfFailed(pDevice->CreateShaderResourceView(
		res->m_resource.Get(),
		&shaderResourceViewDesc,
		res->m_SRV.ReleaseAndGetAddressOf()
	));

	CD3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc(D3D11_DSV_DIMENSION_TEXTURE2D, DXGI_FORMAT_D32_FLOAT);
	DX::ThrowIfFailed(pDevice->CreateDepthStencilView(
		res->m_resource.Get()
		, &dsvDesc
		, res->m_DSV.ReleaseAndGetAddressOf()
	));

	res->Initialize(width, height, 0, DXGI_FORMAT_R32_TYPELESS, TRUE);
	return res;
}

D3D11TextureRender* D3D11ResourceManager::CreateTextureRender(const DXGI_FORMAT format, const UINT width, const UINT height)
{
	D3D11TextureRender* res = new D3D11TextureRender(format);

	auto* pDevice = m_pRenderer->GetDeviceResources()->GetD3DDevice();
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

	DX::ThrowIfFailed(pDevice->CreateTexture2D(
		&desc,
		nullptr,
		res->m_resource.ReleaseAndGetAddressOf()
	));

	// Create RTV.
	CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2D, format);

	DX::ThrowIfFailed(pDevice->CreateRenderTargetView(
		res->m_resource.Get(),
		&renderTargetViewDesc,
		res->m_RTV.ReleaseAndGetAddressOf()
	));

	// Create SRV.
	CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(D3D11_SRV_DIMENSION_TEXTURE2D, format);

	DX::ThrowIfFailed(pDevice->CreateShaderResourceView(
		res->m_resource.Get(),
		&shaderResourceViewDesc,
		res->m_SRV.ReleaseAndGetAddressOf()
	));

	CD3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc(D3D11_UAV_DIMENSION_TEXTURE2D, format);
	DX::ThrowIfFailed(pDevice->CreateUnorderedAccessView(res->m_resource.Get(), NULL, res->m_UAV.ReleaseAndGetAddressOf()));
	res->Initialize(width, height, 0, format, FALSE);
	return res;
}

D3D11Texture2D* D3D11ResourceManager::CreateTexture2DFromWICFile(const WCHAR* fileName)
{
	auto* pDevice = m_pRenderer->GetDeviceResources()->GetD3DDevice();

	D3D11Texture2D* res = new D3D11Texture2D();
	ComPtr<ID3D11Resource> resource;
	DX::ThrowIfFailed(CreateWICTextureFromFileEx(pDevice, fileName, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_FLAG(false), DirectX::WIC_LOADER_DEFAULT, resource.ReleaseAndGetAddressOf(), res->m_SRV.ReleaseAndGetAddressOf()));

	if (SUCCEEDED(resource->QueryInterface(res->m_resource.ReleaseAndGetAddressOf())))
	{
		D3D11_TEXTURE2D_DESC desc{};
		res->m_resource->GetDesc(&desc);
		res->Initialize(desc.Width, desc.Height, 0, desc.Format, TRUE);
	}
	else
	{
		MY_ASSERT(FALSE); // Not 2D Tex?
	}

	return res;
}

D3D11Texture2D* D3D11ResourceManager::CreateTexture2DFromDDSFile(const WCHAR* fileName)
{
	auto* pDevice = m_pRenderer->GetDeviceResources()->GetD3DDevice();
	D3D11Texture2D* res = new D3D11Texture2D();
	ComPtr<ID3D11Resource> resource;

	DX::ThrowIfFailed(CreateDDSTextureFromFileEx(pDevice, fileName, 0, D3D11_USAGE_DEFAULT, D3D10_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_FLAG(false), DDS_LOADER_DEFAULT, resource.ReleaseAndGetAddressOf(), res->m_SRV.ReleaseAndGetAddressOf(), nullptr));

	if (SUCCEEDED(resource->QueryInterface(res->m_resource.ReleaseAndGetAddressOf())))
	{
		D3D11_TEXTURE2D_DESC desc{};
		res->m_resource->GetDesc(&desc);
		res->Initialize(desc.Width, desc.Height, 0, desc.Format, TRUE);
	}
	else
	{
		MY_ASSERT(FALSE); // Not 2D Tex?
	}
	return res;
}

D3D11TextureCube* D3D11ResourceManager::CreateTextureCubeFromDDSFile(const WCHAR* fileName)
{
	auto* pDevice = m_pRenderer->GetDeviceResources()->GetD3DDevice();
	D3D11TextureCube* res = new D3D11TextureCube();
	ComPtr<ID3D11Resource> resource;

	DX::ThrowIfFailed(CreateDDSTextureFromFileEx(pDevice, fileName, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D10_RESOURCE_MISC_FLAG(false), DDS_LOADER_DEFAULT, resource.ReleaseAndGetAddressOf(), res->m_SRV.GetAddressOf(), nullptr));
	res->Initialize(0, 0, 0, DXGI_FORMAT_UNKNOWN, TRUE);
	return res;
}

D3D11MeshGeometry* D3D11ResourceManager::CreateCube()
{
	MeshData md = geometryGenerator::MakeBox(1.f);
	return CreateMeshGeometry(md.verticies.data(), sizeof(Vertex), md.verticies.size(), md.indicies.data(), sizeof(UINT), md.indicies.size());
}

D3D11MeshGeometry* D3D11ResourceManager::CreateSphere()
{
	MeshData md = geometryGenerator::MakeSphere(1.f, 30, 30);

	return CreateMeshGeometry(md.verticies.data(), sizeof(Vertex), md.verticies.size(), md.indicies.data(), sizeof(UINT), md.indicies.size());
}

D3D11MeshGeometry* D3D11ResourceManager::CreateQuad()
{
	MeshData md = geometryGenerator::MakeSquare(1.f);
	return CreateMeshGeometry(md.verticies.data(), sizeof(Vertex), md.verticies.size(), md.indicies.data(), sizeof(UINT), md.indicies.size());
}

D3D11MeshGeometry* D3D11ResourceManager::CreateTessellatedQuad()
{
	MeshData md;
	geometryGenerator::MakeCWQuadPatches(128, &md);

	return CreateMeshGeometry(md.verticies.data(), sizeof(Vertex), md.verticies.size(), md.indicies.data(), sizeof(UINT), md.indicies.size());
}

void D3D11ResourceManager::CreateConstantBuffer(const UINT bufferSize, const void* pInitData, ID3D11Buffer** ppOutBuffer)
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
		DX::ThrowIfFailed(pDevice->CreateBuffer(&desc, NULL, ppOutBuffer));
	}
	else
	{
		D3D11_SUBRESOURCE_DATA initDataDesc;
		ZeroMemory(&initDataDesc, sizeof(initDataDesc));
		initDataDesc.pSysMem = pInitData;
		initDataDesc.SysMemPitch = 0;
		initDataDesc.SysMemSlicePitch = 0;

		DX::ThrowIfFailed(pDevice->CreateBuffer(&desc, &initDataDesc, ppOutBuffer));
	}

}
void D3D11ResourceManager::UpdateConstantBuffer(const UINT bufferSize, const void* pData, ID3D11Buffer* pBuffer)
{
	auto* pContext = m_pRenderer->GetDeviceResources()->GetD3DDeviceContext();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	DX::ThrowIfFailed(pContext->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

	memcpy(mappedResource.pData, pData, bufferSize);

	pContext->Unmap(pBuffer, 0);
}

D3D11StructuredBuffer* D3D11ResourceManager::CreateStructuredBuffer(const UINT bufferSize, const UINT byteStride, const void* pInitData)
{
	D3D11StructuredBuffer* sb = new D3D11StructuredBuffer();
	auto* pDevice = m_pRenderer->GetDeviceResources()->GetD3DDevice();

	D3D11_BUFFER_DESC sbDesc;
	ZeroMemory(&sbDesc, sizeof(sbDesc));
	sbDesc.ByteWidth = bufferSize;
	sbDesc.Usage = D3D11_USAGE_DYNAMIC;
	sbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	sbDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	sbDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	sbDesc.StructureByteStride = byteStride;

	if (pInitData == nullptr)
	{
		DX::ThrowIfFailed(pDevice->CreateBuffer(&sbDesc, nullptr, sb->m_buffer.ReleaseAndGetAddressOf()));
	}
	else
	{
		D3D11_SUBRESOURCE_DATA initData;
		ZeroMemory(&initData, sizeof(initData));
		initData.pSysMem = pInitData;
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		DX::ThrowIfFailed(pDevice->CreateBuffer(&sbDesc, &initData, sb->m_buffer.ReleaseAndGetAddressOf()));
	}


	D3D11_BUFFER_DESC descBuf = {};
	(sb->m_buffer.Get())->GetDesc(&descBuf);
	const UINT BUFFER_TOTAL_SIZE = descBuf.ByteWidth;
	const UINT BUFFER_ELEMENT_SIZE = descBuf.StructureByteStride;
	const UINT BUFFER_LENGTH = BUFFER_TOTAL_SIZE / BUFFER_ELEMENT_SIZE;

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
			desc.BufferEx.NumElements = BUFFER_LENGTH - 0;
		}
		else
		{
			DX::ThrowIfFailed(E_INVALIDARG);
		}
	}

	DX::ThrowIfFailed(pDevice->CreateShaderResourceView(sb->m_buffer.Get(), &desc, sb->m_SRV.ReleaseAndGetAddressOf()));

	return sb;
}

void D3D11ResourceManager::UpdateStructuredBuffer(const UINT uElementSize, const UINT uCount, const void* pData, D3D11StructuredBuffer* pInBuffer)
{
	auto* pContext = m_pRenderer->GetDeviceResources()->GetD3DDeviceContext();
	auto* pDevice = m_pRenderer->GetDeviceResources()->GetD3DDevice();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	DX::ThrowIfFailed(pContext->Map(pInBuffer->m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
	for (UINT i = 0; i < uCount; ++i)
	{
		const void* pDataToWrite = static_cast<const char*>(pData) + (i * uElementSize);
		void* pMapped = static_cast<char*>(mappedResource.pData) + (i * uElementSize);

		std::memcpy(pMapped, pDataToWrite, uElementSize);
	}

	pContext->Unmap(pInBuffer->m_buffer.Get(), 0);

}

void D3D11ResourceManager::InitializeCommonResource() const
{
	Graphics::D3D11::InitCommonResources(m_pRenderer);
}
