#include "pch.h"
#include "ModelMeshPart.h"



/*
	reference : honglab introduction to graphics course
*/
namespace GeometryGenerator
{
	using DirectX::SimpleMath::Vector2;
	using DirectX::SimpleMath::Vector3;
	using DirectX::SimpleMath::Matrix;

	MeshData MakeSphere(const float radius, const unsigned int numSlices, const unsigned int numStacks)
	{
		const float dTheta = -DirectX::XM_2PI / float(numSlices);
		const float dPhi = -DirectX::XM_PI / float(numStacks);

		MeshData meshData;
		meshData.verticies.reserve(numSlices * numStacks + 1);

		std::vector<Vertex>& verticies = meshData.verticies;

		// creating verticies per slice rotating 
		for (unsigned int j = 0; j <= numStacks; j++)
		{
			DirectX::SimpleMath::Vector3 stackStartPoint = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.f, -radius, 0.f), DirectX::SimpleMath::Matrix::CreateRotationZ(dPhi * j));

			for (unsigned int i = 0; i <= numSlices; i++)
			{
				Vertex v;

				v.position = DirectX::SimpleMath::Vector3::Transform(stackStartPoint, DirectX::SimpleMath::Matrix::CreateRotationY(dTheta * float(i)));

				v.normalModel = v.position;
				v.normalModel.Normalize();
				v.textureCoordination = DirectX::SimpleMath::Vector2(float(i) / numSlices, 1.f - float(j) / numStacks);

				DirectX::SimpleMath::Vector3 biTangent = DirectX::SimpleMath::Vector3(0.f, 1.f, 0.f);

				DirectX::SimpleMath::Vector3 normalOrth = v.normalModel - biTangent.Dot(v.normalModel) * v.normalModel;
				normalOrth.Normalize();

				verticies.push_back(v);
			}
		}

		std::vector<uint32_t>& indices = meshData.indicies;

		for (unsigned int j = 0; j < numStacks; ++j)
		{
			const unsigned int offset = (numSlices + 1) * j;
			for (unsigned int i = 0; i < numSlices; ++i)
			{
				indices.push_back(offset + i);
				indices.push_back(offset + i + numSlices + 1);
				indices.push_back(offset + i + 1 + numSlices + 1);

				indices.push_back(offset + i);
				indices.push_back(offset + i + 1 + numSlices + 1);
				indices.push_back(offset + i + 1);
			}
		}

		return meshData;
	}
	MeshData MakeSquare()
	{
		std::vector<Vector3> positions;
		std::vector<Vector3> colors;
		std::vector<Vector3> normals;
		std::vector<Vector2> texcoords; // �ؽ��� ��ǥ

		const float scale = 1.0f;

		// �ո�
		positions.push_back(Vector3(-1.0f, 1.0f, 0.0f) * scale);
		positions.push_back(Vector3(1.0f, 1.0f, 0.0f) * scale);
		positions.push_back(Vector3(1.0f, -1.0f, 0.0f) * scale);
		positions.push_back(Vector3(-1.0f, -1.0f, 0.0f) * scale);
		colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
		colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
		colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
		colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
		normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
		normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
		normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
		normals.push_back(Vector3(0.0f, 0.0f, -1.0f));

		// Texture Coordinates (Direct3D 9)
		// https://learn.microsoft.com/en-us/windows/win32/direct3d9/texture-coordinates
		texcoords.push_back(Vector2(0.0f, 0.0f));
		texcoords.push_back(Vector2(1.0f, 0.0f));
		texcoords.push_back(Vector2(1.0f, 1.0f));
		texcoords.push_back(Vector2(0.0f, 1.0f));

		MeshData meshData;

		for (size_t i = 0; i < positions.size(); i++)
		{
			Vertex v;
			v.position = positions[i];
			v.normalModel = normals[i];
			v.textureCoordination = texcoords[i];

			// v.color = colors[i];

			meshData.verticies.push_back(v);
		}
		meshData.indicies = {
			0, 1, 2, 0, 2, 3, // �ո�
		};

		return meshData;
	}

	MeshData MakeBox(const float scale)
	{

		std::vector<Vector3> positions;
		std::vector<Vector3> colors;
		std::vector<Vector3> normals;
		std::vector<Vector2> texcoords; // �ؽ��� ��ǥ

		// ����
		positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
		positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
		positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
		positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
		colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
		colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
		colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
		colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
		normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
		normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
		normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
		normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
		texcoords.push_back(Vector2(0.0f, 0.0f));
		texcoords.push_back(Vector2(1.0f, 0.0f));
		texcoords.push_back(Vector2(1.0f, 1.0f));
		texcoords.push_back(Vector2(0.0f, 1.0f));

		// �Ʒ���
		positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
		positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
		positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
		positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
		colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
		colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
		colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
		colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
		normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
		normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
		normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
		normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
		texcoords.push_back(Vector2(0.0f, 0.0f));
		texcoords.push_back(Vector2(1.0f, 0.0f));
		texcoords.push_back(Vector2(1.0f, 1.0f));
		texcoords.push_back(Vector2(0.0f, 1.0f));

		// �ո�
		positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
		positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
		positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
		positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
		colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
		colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
		colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
		colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
		normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
		normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
		normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
		normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
		texcoords.push_back(Vector2(0.0f, 0.0f));
		texcoords.push_back(Vector2(1.0f, 0.0f));
		texcoords.push_back(Vector2(1.0f, 1.0f));
		texcoords.push_back(Vector2(0.0f, 1.0f));

		// �޸�
		positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
		positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
		positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
		positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
		colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
		colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
		colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
		colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
		normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
		normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
		normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
		normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
		texcoords.push_back(Vector2(0.0f, 0.0f));
		texcoords.push_back(Vector2(1.0f, 0.0f));
		texcoords.push_back(Vector2(1.0f, 1.0f));
		texcoords.push_back(Vector2(0.0f, 1.0f));

		// ����
		positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
		positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
		positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
		positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
		colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
		colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
		colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
		colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
		normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
		normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
		normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
		normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
		texcoords.push_back(Vector2(0.0f, 0.0f));
		texcoords.push_back(Vector2(1.0f, 0.0f));
		texcoords.push_back(Vector2(1.0f, 1.0f));
		texcoords.push_back(Vector2(0.0f, 1.0f));

		// ������
		positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
		positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
		positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
		positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
		colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
		colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
		colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
		colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
		normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
		normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
		normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
		normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
		texcoords.push_back(Vector2(0.0f, 0.0f));
		texcoords.push_back(Vector2(1.0f, 0.0f));
		texcoords.push_back(Vector2(1.0f, 1.0f));
		texcoords.push_back(Vector2(0.0f, 1.0f));

		MeshData meshData;
		for (size_t i = 0; i < positions.size(); i++)
		{
			Vertex v;
			v.position = positions[i];
			v.normalModel = normals[i];
			v.textureCoordination = texcoords[i];
			meshData.verticies.push_back(v);
		}

		meshData.indicies = {
			0,  1,  2,  0,  2,  3,  // ����
			4,  5,  6,  4,  6,  7,  // �Ʒ���
			8,  9,  10, 8,  10, 11, // �ո�
			12, 13, 14, 12, 14, 15, // �޸�
			16, 17, 18, 16, 18, 19, // ����
			20, 21, 22, 20, 22, 23  // ������
		};

		return meshData;
	}
}

// �߰��� �޽��� ����� ���� �����Ƿ� �׳� ������ �̿�
ModelMeshPart::ModelMeshPart(MeshData& mesh, ID3D11Device1* device)
{
	m_VertexStride = sizeof(Vertex);
	m_VertexOffset = 0;

	m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_IndexFormat = DXGI_FORMAT_R32_UINT;
	m_IndexCount = static_cast<UINT>(mesh.indicies.size());
	// init vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.Usage = D3D11_USAGE_DEFAULT; // read only?
	bufferDesc.ByteWidth = sizeof(Vertex) * mesh.verticies.size();
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0; // no access
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = mesh.verticies.data(); // real data!
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0; // ������ ������?

	HRESULT hr = device->CreateBuffer(&bufferDesc, &InitData, &m_VertexBuffer);
	DX::ThrowIfFailed(hr);

	// index buffer
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(uint32_t) * mesh.indicies.size();
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = mesh.indicies.data();
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0; // ������ ������?

	hr = device->CreateBuffer(&bufferDesc, &InitData, &m_IndexBuffer);
	DX::ThrowIfFailed(hr);
}

ModelMeshPart::ModelMeshPart(const ModelMeshPart& other)
{
	m_VertexStride = other.m_VertexStride;
	m_VertexOffset = other.m_VertexOffset;
	m_IndexCount = other.m_IndexCount;
	m_PrimitiveTopology = other.m_PrimitiveTopology;
	m_IndexFormat = other.m_IndexFormat;

	m_VertexBuffer = other.m_VertexBuffer.Get();
	m_IndexBuffer = other.m_IndexBuffer.Get();

}

void ModelMeshPart::Draw(ID3D11DeviceContext1* context) const
{
	context->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &m_VertexStride, &m_VertexOffset);
	context->IASetIndexBuffer(m_IndexBuffer.Get(), m_IndexFormat, 0);

	context->IASetPrimitiveTopology(m_PrimitiveTopology);

	context->DrawIndexed(m_IndexCount, 0, 0);
}
