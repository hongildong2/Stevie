#include "pch.h"

#include <algorithm>

#include "BasicGeometry.h"

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

				v.position = DirectX::SimpleMath::Vector3::Transform(stackStartPoint, DirectX::SimpleMath::Matrix::CreateRotationY(dTheta * float(i)));

				v.normalModel = v.position;
				v.normalModel.Normalize();
				v.textureCoordination = DirectX::SimpleMath::Vector2(float(i) / numSlices, 1.0f - float(j) / numStacks);

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
			v.position = positions[i];
			v.normalModel = normals[i];
			v.textureCoordination = texcoords[i];
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
			v.position = positions[i];
			v.normalModel = normals[i];
			v.textureCoordination = texcoords[i];
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
		unsigned int currentLevel = splitLevel > 32 ? 32 : splitLevel;
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
				quadTopLeft.position = topLeft + Vector3(dx * i, -dy * j, 0.f); // j grows -y direction
				quadTopLeft.textureCoordination = Vector2(du * i, dv * j); // ij and uv grows same direction

				outMesh->verticies.push_back(quadTopLeft);
				outMesh->indicies.push_back(quadTopLeftIndex);

				// for offeset {00, 10, 01, -10} apply delta

				// generate clockwise
				Vertex quadTopRight = quadTopLeft;
				quadTopRight.position = quadTopLeft.position + Vector3(dx, 0.f, 0.f);
				quadTopRight.textureCoordination = quadTopLeft.textureCoordination + Vector2(du, 0.f);
				outMesh->verticies.push_back(quadTopRight);
				outMesh->indicies.push_back(quadTopLeftIndex + 1);

				Vertex quadBottomRight = quadTopRight;
				quadBottomRight.position = quadTopRight.position + Vector3(0.f, -dy, 0.f);
				quadBottomRight.textureCoordination = quadTopRight.textureCoordination + Vector2(0.f, dv);
				outMesh->verticies.push_back(quadBottomRight);
				outMesh->indicies.push_back(quadTopLeftIndex + 2);

				Vertex quadBottomLeft = quadBottomRight;
				quadBottomLeft.position = quadBottomRight.position + Vector3(-dx, 0.f, 0.f);
				quadBottomLeft.textureCoordination = quadBottomRight.textureCoordination + Vector2(-du, 0.f);
				outMesh->verticies.push_back(quadBottomLeft);
				outMesh->indicies.push_back(quadTopLeftIndex + 3);
			}
		}
	}
}
