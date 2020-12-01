#pragma once

#include "PCH.h"

#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"

class Skybox
{
private:
	Texture texture;
	Shader shader;
	Mesh cube;

public:
	Skybox(Texture&& in_texture, Shader&& in_shader, Mesh&& in_cube);
	~Skybox() = default;

	void SetViewMatrix(const gm::mat4& view);

	void Draw();

	Shader& GetShader(){ return shader; }
};

