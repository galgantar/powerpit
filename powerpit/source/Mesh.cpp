#include "PCH.h"

#include "Mesh.h"

#include "Texture.h"
#include "Shader.h"
#include "SafeCall.h"
#include "Primitives.h"

Mesh::Mesh(std::vector<Vertex>&& in_vertices, std::vector<unsigned int>&& in_indices, std::vector<Texture>&& in_textures)
	: 
		vertices(std::move(in_vertices)),
		indices (std::move(in_indices )),
		textures(std::move(in_textures))
{
	GenerateVertexArray();
}

Mesh::Mesh(Primitive&& p)
	:
	vertices(std::move(p.vertices)),
	indices(std::move(p.indices))
{
	GenerateVertexArray();
}



void Mesh::GenerateVertexArray()
{
	GLcall(glGenBuffers(1, &VBO));
	GLcall(glGenBuffers(1, &EBO));
	GLcall(glGenVertexArrays(1, &VAO));

	GLcall(glBindVertexArray(VAO));

	GLcall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	GLcall(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW));

	GLcall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
	GLcall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW));

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
	glEnableVertexAttribArray(2);

	GLcall(glBindVertexArray(0));
}


Mesh::Mesh(Mesh&& old) noexcept
	:
	vertices(std::move(old.vertices)),
	indices(std::move(old.indices)),
	textures(std::move(old.textures)),

	VAO(std::move(old.VAO)),
	VBO(std::move(old.VBO)),
	EBO(std::move(old.EBO))
{
	old.VAO = -1;
	old.VBO = -1;
	old.EBO = -1;
}

Mesh::~Mesh()
{
	if (VAO != -1)
		glDeleteVertexArrays(1, &VAO);
	if (VBO != -1)
		glDeleteBuffers(1, &VBO);
	if (EBO != -1)
		glDeleteBuffers(1, &EBO);
}
