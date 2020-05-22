#include "PCH.h"

#include "Model.h"
#include "Mesh.h"
#include "SafeCall.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>


Model::Model(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "Assimp can't load a file specified " << path 
			<< " [" << importer.GetErrorString() << "]" << std::endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));

	ProcessNode(*scene->mRootNode, *scene);
	std::cout << "Done loading model" << std::endl;
}

void Model::ProcessNode(const aiNode& node, const aiScene& scene)
{	
	for (int i = 0; i < node.mNumMeshes; ++i) {
		aiMesh* mesh = scene.mMeshes[node.mMeshes[i]];
		meshes.push_back(ProcessMesh(*mesh, scene));
	}
	
	for (int i = 0; i < node.mNumChildren; ++i) {
		ProcessNode(*node.mChildren[i], scene);
	}
	std::cout << "Processed node: " << std::string(node.mName.C_Str()) << std::endl;
}

Mesh Model::ProcessMesh(const aiMesh& mesh, const aiScene& scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (int i = 0; i < mesh.mNumVertices; ++i) {
		Vertex vertex;
		
		// Position
		vertex.position.x = mesh.mVertices[i].x;
		vertex.position.y = mesh.mVertices[i].y;
		vertex.position.z = mesh.mVertices[i].z;
		
		// Normal
		vertex.normal.x = mesh.mNormals[i].x;
		vertex.normal.y = mesh.mNormals[i].y;
		vertex.normal.z = mesh.mNormals[i].z;

		// TexCoords
		if (mesh.mTextureCoords[0]) {
			vertex.texCoord.x = mesh.mTextureCoords[0][i].x;
			vertex.texCoord.y = mesh.mTextureCoords[0][i].y;
		}
		else {
			vertex.texCoord = gm::vec2(0.f);
		}

		vertices.push_back(vertex);
	}

	for (int i = 0; i < mesh.mNumFaces; ++i) {
		aiFace face = mesh.mFaces[i];
		for (int j = 0; j < face.mNumIndices; ++j) {
			indices.push_back(face.mIndices[j]);
		}
	}

	if (mesh.mMaterialIndex >= 0) {
		aiMaterial* material = scene.mMaterials[mesh.mMaterialIndex];

		LoadMaterialTextures(textures, *material, aiTextureType_DIFFUSE, "texture_diffuse");
		LoadMaterialTextures(textures, *material, aiTextureType_SPECULAR, "texture_specular");
	}

	return Mesh(vertices, indices, textures);
}

void Model::LoadMaterialTextures(std::vector<Texture>& dest, const aiMaterial& material, aiTextureType aitype, const std::string& typeName)
{
	for (int i = 0; i < material.GetTextureCount(aitype); ++i) {
		aiString str;
		material.GetTexture(aitype, i, &str);
		std::string name = std::string(str.C_Str());

		if (texturesLoaded.find(name) == texturesLoaded.end()) {
			std::cout << "pushing texture" << std::endl;
			dest.push_back(Texture(directory + '/' + name, true, typeName));
			texturesLoaded.insert(name);
		}
	}
}

void Model::Draw(Shader& shader)
{
	for (Mesh& mesh : meshes)
		mesh.Draw(shader);
}
