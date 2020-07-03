#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in mat4 instanceModelMatrix;

layout(std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

out VS_OUT{
	vec3 Normal;
	vec3 fragPos;
	vec2 texCoord;
} vs_out;


void main()
{
	gl_Position = projection * view * instanceModelMatrix * vec4(aPos, 1.0);
	
	vs_out.fragPos = vec3(instanceModelMatrix * vec4(aPos, 1.0));
	vs_out.Normal = mat3(transpose(inverse(instanceModelMatrix))) * aNormal;
	vs_out.texCoord = vec2(aTexCoord.x, aTexCoord.y * -1.0);
}