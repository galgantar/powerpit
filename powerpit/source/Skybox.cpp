#include "PCH.h"

#include "Skybox.h"
#include "Mesh.h"
#include "Primitives.h"
#include "Renderer.h"

Skybox::Skybox(Texture&& in_texture, Shader&& in_shader, Mesh&& in_cube)
	:
	texture(std::move(in_texture)),
	shader(std::move(in_shader)),
	cube(std::move(in_cube))
{

}

void Skybox::SetViewMatrix(const gm::mat4& view)
{
	gm::mat4 sky_view = view;
	
	sky_view[3][0] = 0.f;
	sky_view[3][1] = 0.f;
	sky_view[3][2] = 0.f;
	sky_view[0][3] = 0.f;
	sky_view[1][3] = 0.f;
	sky_view[2][3] = 0.f;

	shader.SetUniformMat4f("sky_view", sky_view);
}

void Skybox::Draw()
{
	texture.Bind(0);
	shader.SetUniform1i("skybox", 0);

	Renderer::Get().Draw(cube, shader);
}