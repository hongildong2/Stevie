#pragma once
#include "pch.h"

// Copyright https://honglab.co.kr/
// 
// vcpkg install assimp:x64-windows
// Preprocessor definitions¿¡ NOMINMAX Ãß°¡
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>

#include "SubModules/Render/Mesh/MeshCommon.h"

class Model;
class ModelLoader final
{
public:
	ModelLoader(Model* pOutModel, ID3D11Device1* pDevice);
	~ModelLoader() = default;

	ModelLoader(const ModelLoader& other) = delete;
	ModelLoader& operator=(const ModelLoader& other) = delete;

	bool Load(const std::string& basePath, const std::string& filename, bool revertNormals);


private:
	// TODO :: ProcessMesh -> MeshPart, ProcessNode-> Mesh(composed of MeshParts)
	void ProcessNode(const std::string& basePath, aiNode* node, const aiScene* scene, DirectX::SimpleMath::Matrix tr);
	void ProcessMesh(const std::string& basePath, aiMesh* mesh, const aiScene* scene, MeshData* pOutMeshData, TextureFiles* pOutTextureFiles);
	const std::wstring readFilename(const std::string& basePath, aiMaterial* pMaterial, aiTextureType type);

private:
	Model* m_pOutModel;
	ID3D11Device1* m_pDevice;

	bool m_isGLTF = false; // gltf or fbx
	bool m_revertNormals = false;
};
