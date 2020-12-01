#include "PCH.h"


#include "Renderer.h"

#include "Model.h"
#include "Mesh.h"
#include "SafeCall.h"
#include "Buffer.h"


Renderer& Renderer::Get()
{
	static Renderer r;
	return r;
}


void Renderer::Draw(const Mesh& mesh, Shader& shader)
{
	if (mesh.textures.size() >= 1) {
		mesh.textures[0].Bind(0);
		if (shader.HasUniform("material.diffuseMap"))
			shader.SetUniform1i("material.diffuseMap", 0);
	}

	if (mesh.textures.size() >= 2) {
		mesh.textures[1].Bind(1);
		if (shader.HasUniform("material.specularMap"))
			shader.SetUniform1i("material.specularMap", 1);
	}
	
	if (shader.HasUniform("material.shininess"))
		shader.SetUniform1f("material.shininess", 32.f);

	shader.Bind();
	GLcall(glBindVertexArray(mesh.VAO));
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO); UNNECESSARY
	GLcall(glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0));
	GLcall(glBindVertexArray(0));
	shader.Unbind();
}

void Renderer::Draw(const Model& model, Shader& shader)
{
	for (const Mesh& m : model.meshes)
	{
		Draw(m, shader);
	}
}

Buffer Renderer::SetUpInstancedTransformations(Mesh& mesh, const std::vector<gm::mat4>& model_matrices)
{
	size_t buffer_size = model_matrices.size() * sizeof(gm::mat4);
	
	Buffer pos_buffer = Buffer(GL_ARRAY_BUFFER, buffer_size, GL_STATIC_DRAW);
	pos_buffer.CopyData(0, buffer_size * 64, &model_matrices[0]);
	pos_buffer.Bind();
	glBindVertexArray(mesh.GetVAO());
	
	// split 4x4 matrix into 4 vec4s
	size_t vec4Size = sizeof(gm::vec4);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));
	glEnableVertexAttribArray(6);
	
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	
	pos_buffer.Unbind();
	glBindVertexArray(0);

	return std::move(pos_buffer);
}

void Renderer::DrawInstanced(Mesh& mesh, Shader& shader, const unsigned int nr_of_objects)
{
	shader.SetUniform1i("material.diffuseMap", 0);
	GLcall(glBindVertexArray(mesh.GetVAO()));
	shader.Bind();
	glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, pow(75, 3));
}