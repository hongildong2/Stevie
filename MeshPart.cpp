#include "pch.h"
#include "MeshPart.h"

// 중간에 메쉬를 만드는 경우는 없으므로 그냥 생성자 이용
MeshPart::MeshPart(MeshData& mesh, const EMeshType type, ID3D11Device1* pDevice, const TextureFiles& tex)
	: m_type(type),
	m_meshConstants{ DirectX::SimpleMath::Matrix(), DirectX::SimpleMath::Matrix() , DirectX::SimpleMath::Matrix(), FALSE, 1.f, 0, 0.f },
	m_materialConstants(DEFAULT_MATERIAL),
	m_vertexStride(sizeof(Vertex)),
	m_vertexOffset(0),
	m_indexFormat(DXGI_FORMAT_R32_UINT),
	m_indexCount(static_cast<UINT>(mesh.indicies.size()))
{
	// init vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.Usage = D3D11_USAGE_DEFAULT; // read only?
	bufferDesc.ByteWidth = sizeof(Vertex) * static_cast<UINT>(mesh.verticies.size());
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0; // no access
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = mesh.verticies.data(); // real data!
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0; // 사이즈 보정용?

	HRESULT hr = pDevice->CreateBuffer(&bufferDesc, &InitData, &m_vertexBuffer);
	DX::ThrowIfFailed(hr);

	// index buffer
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(uint32_t) * static_cast<UINT>(mesh.indicies.size());
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = mesh.indicies.data();
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0; // 사이즈 보정용?

	hr = pDevice->CreateBuffer(&bufferDesc, &InitData, &m_indexBuffer);
	DX::ThrowIfFailed(hr);

	if (tex.albedoName != nullptr)
	{
		// All Textures should be SRGB, linear space
		DX::ThrowIfFailed(
			DirectX::CreateWICTextureFromFileEx(pDevice, tex.albedoName, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_FLAG(false), DirectX::WIC_LOADER_DEFAULT, nullptr, m_albedoView.GetAddressOf()));
		DX::ThrowIfFailed(
			DirectX::CreateWICTextureFromFileEx(pDevice, tex.aoName, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_FLAG(false), DirectX::WIC_LOADER_DEFAULT, nullptr, m_aoview.GetAddressOf()));
		DX::ThrowIfFailed(
			DirectX::CreateWICTextureFromFileEx(pDevice, tex.heightName, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_FLAG(false), DirectX::WIC_LOADER_DEFAULT, nullptr, m_heightView.GetAddressOf()));
		DX::ThrowIfFailed(
			DirectX::CreateWICTextureFromFileEx(pDevice, tex.metallicName, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_FLAG(false), DirectX::WIC_LOADER_DEFAULT, nullptr, m_metallicView.GetAddressOf()));
		DX::ThrowIfFailed(
			DirectX::CreateWICTextureFromFileEx(pDevice, tex.normalName, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_FLAG(false), DirectX::WIC_LOADER_DEFAULT, nullptr, m_normalView.GetAddressOf()));
		DX::ThrowIfFailed(
			DirectX::CreateWICTextureFromFileEx(pDevice, tex.roughnessName, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_FLAG(false), DirectX::WIC_LOADER_DEFAULT, nullptr, m_roughnessView.GetAddressOf()));
	}
}

void MeshPart::Initialize(ID3D11Device1* pDevice)
{

	assert(pDevice != nullptr);

	// 버퍼도 사실 동일한 타입이면 공유해도 되긴함 일단 이렇게
	Utility::DXResource::CreateConstantBuffer(m_meshConstants, pDevice, m_meshCB);
	Utility::DXResource::CreateConstantBuffer(m_materialConstants, pDevice, m_materialCB);
}

void MeshPart::Prepare(ID3D11DeviceContext1* pContext, DirectX::SimpleMath::Matrix& parentWorld)
{
	// my world, relative to parent
	m_meshConstants.world = DirectX::SimpleMath::Matrix::CreateTranslation(m_modelPos) * parentWorld;
	m_meshConstants.worldInv = m_meshConstants.world.Invert();
	m_meshConstants.worldIT = m_meshConstants.worldInv.Transpose();

	// Row -> Column wise
	m_meshConstants.world = m_meshConstants.world.Transpose();
	m_meshConstants.worldInv = m_meshConstants.worldInv.Transpose();
	m_meshConstants.worldIT = m_meshConstants.worldIT.Transpose();

	Utility::DXResource::UpdateConstantBuffer(m_meshConstants, pContext, m_meshCB);
	Utility::DXResource::UpdateConstantBuffer(m_materialConstants, pContext, m_materialCB);
}


void MeshPart::Draw(ID3D11DeviceContext1* pContext) const
{
	pContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &m_vertexStride, &m_vertexOffset);
	pContext->IASetIndexBuffer(m_indexBuffer.Get(), m_indexFormat, 0);

	ID3D11Buffer* meshCBs[2] = { GetMeshCB(), GetMaterialCB() };


	if (m_type == EMeshType::TESSELLATED)
	{
		pContext->HSSetConstantBuffers(1, 2, meshCBs);
		pContext->DSSetConstantBuffers(1, 2, meshCBs);
	}
	else if (m_type == EMeshType::GEOMETRIC)
	{
		pContext->GSSetConstantBuffers(1, 2, meshCBs);
	}

	pContext->VSSetConstantBuffers(1, 2, meshCBs);
	pContext->PSSetConstantBuffers(1, 2, meshCBs);

	if (m_materialConstants.bUseTexture == TRUE && m_albedoView != nullptr)
	{
		ID3D11ShaderResourceView* SRVs[6] = {
		 m_albedoView.Get(),
		 m_aoview.Get(),
		 m_heightView.Get(),
		 m_metallicView.Get(),
		 m_normalView.Get(),
		 m_roughnessView.Get()
		};

		pContext->PSSetShaderResources(30, 6, SRVs);
		pContext->VSSetShaderResources(30, 6, SRVs);
	}



	pContext->DrawIndexed(m_indexCount, 0, 0);
}

void MeshPart::UpdateMaterialConstant(Material& mat)
{
	m_materialConstants = mat;
}
void MeshPart::UpdateMeshConstant(MeshConstants& mesh)
{
	m_meshConstants = mesh;
}
