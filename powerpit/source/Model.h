#pragma once

#include "PCH.h"

#include "Mesh.h"
#include "Shader.h"

#include <assimp/scene.h>

class Model
{
private:
	std::vector<Mesh> meshes;
	std::set<std::string> texturesLoaded;

	std::string directory;

	void ProcessNode(const aiNode& node, const aiScene& scene);
	Mesh ProcessMesh(const aiMesh& mesh, const aiScene& scene);
	void Model::LoadMaterialTextures(std::vector<Texture>& dest, const aiMaterial& material, aiTextureType type, const std::string& typeName);

public:
	Model(const std::string& path);
	~Model() = default;

	void Draw(Shader& shader);
};