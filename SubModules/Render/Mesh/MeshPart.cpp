#include "pch.h"
#include "MeshPart.h"

// 중간에 메쉬를 만드는 경우는 없으므로 그냥 생성자 이용

MeshPart::MeshPart(MeshData& mesh, const EMeshType type, ID3D11Device1* pDevice)
	: m_type(type),
	m_meshConstants{ DirectX::SimpleMath::Matrix(), DirectX::SimpleMath::Matrix() , DirectX::SimpleMath::Matrix(), FALSE, 1.f, 0, 0.f },
	m_materialConstants(DEFAULT_MATERIAL),
	m_vertexStride(sizeof(Vertex)),
	m_vertexOffset(0),
	m_indexFormat(DXGI_FORMAT_R32_UINT),
	m_indexCount(static_cast<UINT>(mesh.indicies.size())),
	m_posModel()
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
	DX::ThrowIfFailed(pDevice->CreateBuffer(&bufferDesc, &InitData, &m_vertexBuffer));

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
	DX::ThrowIfFailed(pDevice->CreateBuffer(&bufferDesc, &InitData, &m_indexBuffer));

}

MeshPart::MeshPart(MeshData& mesh, const EMeshType type, ID3D11Device1* pDevice, const TextureFiles& tex)
	: MeshPart(mesh, type, pDevice)
{
	if (!tex.albedoName.empty())
	{
		// All Textures should be SRGB, linear space
		DX::ThrowIfFailed(
			DirectX::CreateWICTextureFromFileEx(pDevice, tex.albedoName.c_str(), 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_FLAG(false), DirectX::WIC_LOADER_DEFAULT, nullptr, m_albedoView.GetAddressOf()));
	}

	if (!tex.aoName.empty())
	{
		DX::ThrowIfFailed(
			DirectX::CreateWICTextureFromFileEx(pDevice, tex.aoName.c_str(), 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_FLAG(false), DirectX::WIC_LOADER_DEFAULT, nullptr, m_aoview.GetAddressOf()));
	}

	if (!tex.heightName.empty())
	{
		DX::ThrowIfFailed(
			DirectX::CreateWICTextureFromFileEx(pDevice, tex.heightName.c_str(), 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_FLAG(false), DirectX::WIC_LOADER_DEFAULT, nullptr, m_heightView.GetAddressOf()));
	}

	if (!tex.metallicName.empty())
	{
		DX::ThrowIfFailed(
			DirectX::CreateWICTextureFromFileEx(pDevice, tex.metallicName.c_str(), 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_FLAG(false), DirectX::WIC_LOADER_DEFAULT, nullptr, m_metallicView.GetAddressOf()));
	}

	if (!tex.normalName.empty())
	{
		DX::ThrowIfFailed(
			DirectX::CreateWICTextureFromFileEx(pDevice, tex.normalName.c_str(), 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_FLAG(false), DirectX::WIC_LOADER_DEFAULT, nullptr, m_normalView.GetAddressOf()));
	}
	if (!tex.roughnessName.empty())
	{
		DX::ThrowIfFailed(
			DirectX::CreateWICTextureFromFileEx(pDevice, tex.roughnessName.c_str(), 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_FLAG(false), DirectX::WIC_LOADER_DEFAULT, nullptr, m_roughnessView.GetAddressOf()));
	}
	if (!tex.emissiveName.empty())
	{
		DX::ThrowIfFailed(
			DirectX::CreateWICTextureFromFileEx(pDevice, tex.emissiveName.c_str(), 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_FLAG(false), DirectX::WIC_LOADER_DEFAULT, nullptr, m_emissiveView.GetAddressOf()));
	}
	if (!tex.opacityName.empty())
	{
		DX::ThrowIfFailed(
			DirectX::CreateWICTextureFromFileEx(pDevice, tex.opacityName.c_str(), 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_FLAG(false), DirectX::WIC_LOADER_DEFAULT, nullptr, m_opacityView.GetAddressOf()));
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
	auto world = DirectX::SimpleMath::Matrix::CreateTranslation(m_posModel) * parentWorld;
	auto worldInv = world.Invert();
	auto worldIT = world.Invert().Transpose();

	m_meshConstants.world = world.Transpose();
	m_meshConstants.worldInv = worldInv.Transpose();
	m_meshConstants.worldIT = worldIT.Transpose();

	// MeshConstants toSend = m_meshConstants;

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
		ID3D11ShaderResourceView* SRVs[8] =
		{
		 m_albedoView.Get(),
		 m_aoview.Get(),
		 m_heightView.Get(),
		 m_metallicView.Get(),
		 m_normalView.Get(),
		 m_roughnessView.Get(),
		 m_emissiveView.Get(),
		 m_opacityView.Get(),
		};

		pContext->PSSetShaderResources(30, 8, SRVs);
		pContext->VSSetShaderResources(30, 8, SRVs);
	}

	ID3D11ShaderResourceView* release[8] = { NULL, };

	pContext->DrawIndexed(m_indexCount, 0, 0);

	pContext->PSSetShaderResources(30, 8, release);
	pContext->VSSetShaderResources(30, 8, release);
}

void MeshPart::UpdateMaterialConstant(Material& mat)
{
	m_materialConstants = mat;
}
void MeshPart::UpdateMeshConstant(MeshConstants& mesh)
{
	m_meshConstants = mesh;
}
