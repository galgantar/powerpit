#pragma once

#include "PCH.h"
#include "Texture.h"
#include "Shader.h"
#include "Primitives.h"


class Mesh
{
private:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

	void GenerateVertexArray();
	
	// for Renderer class only
public:
	unsigned int GetVAO() { return VAO; }

public:
	Mesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices, std::vector<Texture>&& textures);
	Mesh(Primitive&& p);
	
	Mesh(Mesh&& old) noexcept;
	Mesh(Mesh& m) = delete;
	~Mesh();
	

	friend class Renderer;
};