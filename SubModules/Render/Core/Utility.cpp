#include "pch.h"
#include "Utility.h"

namespace Utility
{
	namespace DXResource
	{
		void CreateStructuredBuffer(ID3D11Device* pDevice, UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBufOut)
		{
			*ppBufOut = nullptr; // ÀÌ°Ç ¿ÖÇÏ´Â°ÅÁö?? ¸Þ¸ð¸® ¾ÈÁ¤¼ºÀ» À§ÇØ¼­?

			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));

			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.ByteWidth = uElementSize * uCount;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.StructureByteStride = uElementSize;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.Usage = D3D11_USAGE_DYNAMIC;

			if (pInitData)
			{
				D3D11_SUBRESOURCE_DATA InitData;
				ZeroMemory(&InitData, sizeof(D3D11_SUBRESOURCE_DATA));
				InitData.pSysMem = pInitData;
				DX::ThrowIfFailed(pDevice->CreateBuffer(&desc, &InitData, ppBufOut));
			}
			else
			{
				DX::ThrowIfFailed(pDevice->CreateBuffer(&desc, nullptr, ppBufOut));
			}
		}

		void CreateStructuredBufferSRV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, UINT subResourceIndex, ID3D11ShaderResourceView** ppSRVOut)
		{
			D3D11_BUFFER_DESC descBuf = {};
			pBuffer->GetDesc(&descBuf);
			const UINT BUFFER_TOTAL_SIZE = descBuf.ByteWidth;
			const UINT BUFFER_ELEMENT_SIZE = descBuf.StructureByteStride;
			const UINT BUFFER_LENGTH = BUFFER_TOTAL_SIZE / BUFFER_ELEMENT_SIZE;
			assert(subResourceIndex < BUFFER_LENGTH);

			D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
			desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
			desc.BufferEx.FirstElement = subResourceIndex;
			if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
			{
				// This is a Raw Buffer

				desc.Format = DXGI_FORMAT_R32_TYPELESS;
				desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
				desc.BufferEx.NumElements = descBuf.ByteWidth / 4;
			}
			else
				if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
				{
					// This is a Structured Buffer
					desc.Format = DXGI_FORMAT_UNKNOWN;
					desc.BufferEx.NumElements = BUFFER_LENGTH - subResourceIndex;
				}
				else
				{
					DX::ThrowIfFailed(E_INVALIDARG);
				}

			DX::ThrowIfFailed(pDevice->CreateShaderResourceView(pBuffer, &desc, ppSRVOut));
		}

		void UpdateBuffer(ID3D11DeviceContext1* pContext, ID3D11Buffer* pBuffer, UINT uElementSize, UINT uCount, void* pData)
		{
			assert(pContext != nullptr && pBuffer != nullptr);

			D3D11_MAPPED_SUBRESOURCE mappedResource;
			ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

			DX::ThrowIfFailed(pContext->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

			// row pitch means aligned byte size of texture's single row, not element size
			// assert(uElementSize == mappedResource.RowPitch);

			for (UINT i = 0; i < uCount; ++i)
			{
				const void* pDataToWrite = static_cast<const char*>(pData) + (i * uElementSize);
				void* pMapped = static_cast<char*>(mappedResource.pData) + (i * uElementSize);

				std::memcpy(pMapped, pDataToWrite, uElementSize);
			}

			pContext->Unmap(pBuffer, 0);
		}
	}

	void ComputeShaderBarrier(ID3D11DeviceContext1* context)
	{
		ID3D11ShaderResourceView* nullSRV[6] = { 0, };
		context->CSSetShaderResources(0, 6, nullSRV);
		ID3D11UnorderedAccessView* nullUAV[6] = { 0, };
		context->CSSetUnorderedAccessViews(0, 6, nullUAV, NULL);
	}


}


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

		// creating verticies per slice rotating 
		for (unsigned int j = 0; j <= numStacks; j++)
		{
			DirectX::SimpleMath::Vector3 stackStartPoint = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.f, -radius, 0.f), DirectX::SimpleMath::Matrix::CreateRotationZ(dPhi * j));

			for (unsigned int i = 0; i <= numSlices; i++)
			{
				Vertex v;

				v.positionModel = DirectX::SimpleMath::Vector3::Transform(stackStartPoint, DirectX::SimpleMath::Matrix::CreateRotationY(dTheta * float(i)));

				v.normalModel = v.positionModel;
				v.normalModel.Normalize();
				v.texcoordinate = DirectX::SimpleMath::Vector2(float(i) / numSlices, 1.0f - float(j) / numStacks);

				DirectX::SimpleMath::Vector3 biTangent = DirectX::SimpleMath::Vector3(0.f, 1.f, 0.f);

				DirectX::SimpleMath::Vector3 normalOrth = v.normalModel - biTangent.Dot(v.normalModel) * v.normalModel;
				normalOrth.Normalize();


				v.tangentModel = biTangent.Cross(normalOrth);
				v.tangentModel.Normalize();

				meshData.verticies.push_back(v);
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

	MeshData MakeSquare(const float scale = 1.0f)
	{
		std::vector<Vector3> positions;
		std::vector<Vector3> colors;
		std::vector<Vector3> normals;
		std::vector<Vector2> texcoords; // ÅØ½ºÃç ÁÂÇ¥

		// ¾Õ¸é
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
			v.positionModel = positions[i];
			v.normalModel = normals[i];
			v.texcoordinate = texcoords[i];
			v.tangentModel = Vector3(0.f, 1.f, 0.f);

			// v.color = colors[i];

			meshData.verticies.push_back(v);
		}
		meshData.indicies = {
			0, 1, 2, 0, 2, 3, // ¾Õ¸é
		};

		return meshData;
	}

	MeshData MakeBox(const float scale)
	{

		std::vector<Vector3> positions;
		std::vector<Vector3> colors;
		std::vector<Vector3> normals;
		std::vector<Vector2> texcoords; // ÅØ½ºÃç ÁÂÇ¥

		// À­¸é
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

		// ¾Æ·§¸é
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

		// ¾Õ¸é
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

		// µÞ¸é
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

		// ¿ÞÂÊ
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

		// ¿À¸¥ÂÊ
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
			v.positionModel = positions[i];
			v.normalModel = normals[i];
			v.texcoordinate = texcoords[i];
			meshData.verticies.push_back(v);
		}

		meshData.indicies = {
			0,  1,  2,  0,  2,  3,  // À­¸é
			4,  5,  6,  4,  6,  7,  // ¾Æ·§¸é
			8,  9,  10, 8,  10, 11, // ¾Õ¸é
			12, 13, 14, 12, 14, 15, // µÞ¸é
			16, 17, 18, 16, 18, 19, // ¿ÞÂÊ
			20, 21, 22, 20, 22, 23  // ¿À¸¥ÂÊ
		};

		return meshData;
	}

	void MakeCWQuadPatches(unsigned int splitLevel, MeshData* const outMesh)
	{
		unsigned int currentLevel = splitLevel > 128 ? 128 : splitLevel;
		Vector3 topLeft(-1.f, 1.f, 0.f);
		Vector3 bottomRight(1.f, -1.f, 0.f);

		outMesh->verticies.reserve(4 * currentLevel * currentLevel + 1);
		outMesh->indicies.reserve(4 * currentLevel * currentLevel + 1);

		const Vector3 NORMAL_MODEL(0.f, 0.f, -1.f);
		const Vector3 TANGENT_MODEL(0.f, 1.f, 0.f);

		float dx = (bottomRight.x - topLeft.x) / splitLevel;
		float dy = (topLeft.y - bottomRight.y) / splitLevel;

		float du = 1.f / splitLevel;
		float dv = 1.f / splitLevel;

		for (unsigned int i = 0; i < splitLevel; ++i)
		{
			for (unsigned int j = 0; j < splitLevel; ++j)
			{
				// instance quad clockwise
				Vertex quadTopLeft;
				unsigned int quadTopLeftIndex = 4 * (i + splitLevel * j); // vertex index, 4 index per quad patch

				quadTopLeft.normalModel = NORMAL_MODEL;
				quadTopLeft.tangentModel = TANGENT_MODEL;
				quadTopLeft.positionModel = topLeft + Vector3(dx * i, -dy * j, 0.f); // j grows -y direction
				quadTopLeft.texcoordinate = Vector2(du * i, dv * j); // ij and uv grows same direction

				outMesh->verticies.push_back(quadTopLeft);
				outMesh->indicies.push_back(quadTopLeftIndex);

				// for offeset {00, 10, 01, -10} apply delta

				// generate clockwise
				Vertex quadTopRight = quadTopLeft;
				quadTopRight.positionModel = quadTopLeft.positionModel + Vector3(dx, 0.f, 0.f);
				quadTopRight.texcoordinate = quadTopLeft.texcoordinate + Vector2(du, 0.f);
				outMesh->verticies.push_back(quadTopRight);
				outMesh->indicies.push_back(quadTopLeftIndex + 1);

				Vertex quadBottomRight = quadTopRight;
				quadBottomRight.positionModel = quadTopRight.positionModel + Vector3(0.f, -dy, 0.f);
				quadBottomRight.texcoordinate = quadTopRight.texcoordinate + Vector2(0.f, dv);
				outMesh->verticies.push_back(quadBottomRight);
				outMesh->indicies.push_back(quadTopLeftIndex + 2);

				Vertex quadBottomLeft = quadBottomRight;
				quadBottomLeft.positionModel = quadBottomRight.positionModel + Vector3(-dx, 0.f, 0.f);
				quadBottomLeft.texcoordinate = quadBottomRight.texcoordinate + Vector2(-du, 0.f);
				outMesh->verticies.push_back(quadBottomLeft);
				outMesh->indicies.push_back(quadTopLeftIndex + 3);
			}
		}
	}
}
