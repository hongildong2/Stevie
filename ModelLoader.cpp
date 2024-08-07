#include "pch.h"

#include <filesystem>
#include <DirectXMesh.h>

#include "ModelLoader.h"

// Copyright https://honglab.co.kr/
using namespace DirectX::SimpleMath;

void updateNormals(std::vector<MeshData>& m_meshes)
{

	// 노멀 벡터가 없는 경우를 대비하여 다시 계산
	// 한 위치에는 한 버텍스만 있어야 연결 관계를 찾을 수 있음

	// DirectXMesh의 ComputeNormals()과 비슷합니다.
	// https://github.com/microsoft/DirectXMesh/wiki/ComputeNormals

	for (auto& m : m_meshes)
	{

		std::vector<Vector3> normalsTemp(m.verticies.size(), Vector3(0.0f));
		std::vector<float> weightsTemp(m.verticies.size(), 0.0f);

		for (unsigned int i = 0; i < m.indicies.size(); i += 3)
		{

			int idx0 = m.indicies[i];
			int idx1 = m.indicies[i + 1];
			int idx2 = m.indicies[i + 2];

			auto v0 = m.verticies[idx0];
			auto v1 = m.verticies[idx1];
			auto v2 = m.verticies[idx2];

			auto faceNormal =
				(v1.positionModel - v0.positionModel).Cross(v2.positionModel - v0.positionModel);

			normalsTemp[idx0] += faceNormal;
			normalsTemp[idx1] += faceNormal;
			normalsTemp[idx2] += faceNormal;
			weightsTemp[idx0] += 1.0f;
			weightsTemp[idx1] += 1.0f;
			weightsTemp[idx2] += 1.0f;
		}

		for (int i = 0; i < m.verticies.size(); i++)
		{
			if (weightsTemp[i] > 0.0f)
			{
				m.verticies[i].normalModel = normalsTemp[i] / weightsTemp[i];
				m.verticies[i].normalModel.Normalize();
			}
		}
	}
}

std::string getExtension(const std::string& filename)
{
	std::string ext(std::filesystem::path(filename).extension().string());

	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
	return ext;
}




void updateTangents(MeshData& mesh)
{

	using namespace std;
	using namespace DirectX;

	// https://github.com/microsoft/DirectXMesh/wiki/ComputeTangentFrame

	vector<XMFLOAT3> positions(mesh.verticies.size());
	vector<XMFLOAT3> normals(mesh.verticies.size());
	vector<XMFLOAT2> texcoords(mesh.verticies.size());
	vector<XMFLOAT3> tangents(mesh.verticies.size());
	vector<XMFLOAT3> bitangents(mesh.verticies.size());

	for (size_t i = 0; i < mesh.verticies.size(); i++)
	{
		auto& v = mesh.verticies[i];
		positions[i] = v.positionModel;
		normals[i] = v.normalModel;
		texcoords[i] = v.texcoordinate;
	}

	ComputeTangentFrame(mesh.indicies.data(), mesh.indicies.size() / 3,
		positions.data(), normals.data(), texcoords.data(),
		mesh.verticies.size(), tangents.data(),
		bitangents.data());

	for (size_t i = 0; i < mesh.verticies.size(); i++)
	{
		mesh.verticies[i].tangentModel = tangents[i];
	}
}

ModelLoader::ModelLoader(Model* pOutModel, ID3D11Device1* pDevice)
	: m_pOutModel(pOutModel)
	, m_pDevice(pDevice)
{
}

bool ModelLoader::Load(const std::string& basePath, const std::string& filename, bool revertNormals)
{
	assert(m_pOutModel != nullptr && m_pDevice != nullptr);
	if (getExtension(filename) == ".gltf")
	{
		m_isGLTF = true;
		m_revertNormals = revertNormals;
	}

	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(basePath + filename, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

	if (!pScene)
	{
		return false;
	}
	else
	{
		Matrix tr; // Initial transformation
		ProcessNode(basePath, pScene->mRootNode, pScene, tr);
	}

	// UpdateNormals(this->meshes); // Vertex Normal을 직접 계산 (참고용)

	m_pOutModel = nullptr;
	m_pDevice = nullptr;
	return true;
}


void ModelLoader::ProcessNode(const std::string& basePath, aiNode* node, const aiScene* scene, Matrix tr)
{
	Matrix accumulatedTransform; // Transforming Meshes by Hierarchy In Model Coordinate
	{
		ai_real* temp = &node->mTransformation.a1;
		float* mTemp = &accumulatedTransform._11;
		for (int t = 0; t < 16; t++)
		{
			mTemp[t] = float(temp[t]);
		}

		accumulatedTransform = accumulatedTransform.Transpose() * tr;
	}

	for (UINT i = 0; i < node->mNumMeshes; i++)
	{

		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		MeshData newMesh;
		TextureFiles texFiles;
		ZeroMemory(&texFiles, sizeof(TextureFiles));
		ProcessMesh(basePath, mesh, scene, &newMesh, &texFiles);

		for (auto& v : newMesh.verticies)
		{
			v.positionModel = DirectX::SimpleMath::Vector3::Transform(v.positionModel, accumulatedTransform);
		}

		updateTangents(newMesh);

		m_pOutModel->AddMeshComponent(std::make_unique<MeshPart>(newMesh, EMeshType::SOLID, m_pDevice, texFiles)); // TODO :: meshComponent->AddMeshParts(ProcessMesh)
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(basePath, node->mChildren[i], scene, accumulatedTransform); // TODO :: m_pOutModel->AddMeshComponent(ProcessNode);
	}
}

const std::wstring ModelLoader::readFilename(const std::string& basePath, aiMaterial* material, aiTextureType type)
{
	std::string path;
	if (material->GetTextureCount(type) > 0)
	{
		aiString filepath;
		material->GetTexture(type, 0, &filepath);

		path = basePath + std::string(std::filesystem::path(filepath.C_Str()).filename().string());
	}
	else
	{
		path = "";
	}

	return std::wstring(path.begin(), path.end());
}

void ModelLoader::ProcessMesh(const std::string& basePath, aiMesh* mesh, const aiScene* scene, MeshData* pOutMeshData, TextureFiles* pOutTextureFiles)
{

	// Data to fill
	std::vector<Vertex>& vertices = pOutMeshData->verticies;
	std::vector<uint32_t>& indices = pOutMeshData->indicies;

	// Walk through each of the mesh's vertices
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		vertex.positionModel.x = mesh->mVertices[i].x;
		vertex.positionModel.y = mesh->mVertices[i].y;
		vertex.positionModel.z = mesh->mVertices[i].z;

		vertex.normalModel.x = mesh->mNormals[i].x;
		if (m_isGLTF)
		{
			vertex.normalModel.y = mesh->mNormals[i].z;
			vertex.normalModel.z = -mesh->mNormals[i].y;
		}
		else
		{
			vertex.normalModel.y = mesh->mNormals[i].y;
			vertex.normalModel.z = mesh->mNormals[i].z;
		}

		if (m_revertNormals)
		{
			vertex.normalModel *= -1.0f;
		}

		vertex.normalModel.Normalize();

		if (mesh->mTextureCoords[0])
		{
			vertex.texcoordinate.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.texcoordinate.y = (float)mesh->mTextureCoords[0][i].y;
		}

		vertices.push_back(vertex);
	}

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (UINT j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// http://assimp.sourceforge.net/lib_html/materials.html
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		auto albedoRead = readFilename(basePath, material, aiTextureType_BASE_COLOR);
		if (albedoRead.empty())
		{
			albedoRead = readFilename(basePath, material, aiTextureType_DIFFUSE);
		}
		pOutTextureFiles->albedoName = albedoRead;


		pOutTextureFiles->emissiveName = readFilename(basePath, material, aiTextureType_EMISSIVE);
		pOutTextureFiles->heightName = readFilename(basePath, material, aiTextureType_HEIGHT);
		pOutTextureFiles->normalName = readFilename(basePath, material, aiTextureType_NORMALS);
		pOutTextureFiles->metallicName = readFilename(basePath, material, aiTextureType_METALNESS);
		pOutTextureFiles->roughnessName = readFilename(basePath, material, aiTextureType_DIFFUSE_ROUGHNESS);


		auto aoRead = readFilename(basePath, material, aiTextureType_AMBIENT_OCCLUSION);
		if (aoRead.empty())
		{
			aoRead = readFilename(basePath, material, aiTextureType_LIGHTMAP);
		}

		pOutTextureFiles->aoName = aoRead;

	}
}