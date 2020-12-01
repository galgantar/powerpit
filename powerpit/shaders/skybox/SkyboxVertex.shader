#version 330 core

layout (location = 0) in vec3 aPos;

layout(std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

out vec3 texCoords;

uniform mat4 sky_view;

void main()
{
	texCoords = aPos;
	vec4 pos = projection * sky_view * vec4(aPos, 1.0);
	gl_Position = pos.xyww;
}