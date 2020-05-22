#pragma once

#include "PCH.h"
#include "VertexArrayObject.h"
#include "Texture.h"
#include "Shader.h"

struct Vertex
{
	gm::vec3 position;
	gm::vec3 normal;
	gm::vec2 texCoord;
};

class Mesh
{
private:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

public:
	Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures);
	Mesh(Mesh&& old) noexcept;
	~Mesh();

	void Draw(Shader& shader);
};