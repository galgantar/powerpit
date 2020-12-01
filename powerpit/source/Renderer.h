#pragma once

#include "Model.h"
#include "Mesh.h"
#include "Buffer.h"

class Renderer
{
public:
	static Renderer& Get();

	Renderer() {};
	~Renderer() {};

	void Draw(const Model& model, Shader& shader);
	void Draw(const Mesh& mesh, Shader& shader);

	Buffer SetUpInstancedTransformations(Mesh& mesh, const std::vector<gm::mat4>& models);

	void DrawInstanced(Mesh& mesh, Shader& shader, const unsigned int nr_of_objects);
};