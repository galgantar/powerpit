#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VS_OUT {
	vec3 fragPos;
	vec3 Normal;
	vec2 texCoord;
} gs_in[];


out vec3 fragPos;
out vec3 Normal;
out vec2 texCoord;

uniform float deltaTime;


vec3 getNormal()
{
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	return normalize(cross(a, b));
}

void main()
{
	gl_Position = gl_in[0].gl_Position + vec4(getNormal() * (sin(deltaTime) + 1.0) / 2.0 * 2.0, 1.0);
	fragPos = gs_in[0].fragPos;
	Normal = gs_in[0].Normal;
	texCoord = gs_in[0].texCoord;
	EmitVertex();
	gl_Position = gl_in[1].gl_Position + vec4(getNormal() * (sin(deltaTime) + 1.0) / 2.0 * 2.0, 1.0);
	fragPos = gs_in[1].fragPos;
	Normal = gs_in[1].Normal;
	texCoord = gs_in[1].texCoord;
	EmitVertex();
	gl_Position = gl_in[2].gl_Position + vec4(getNormal() * (sin(deltaTime) + 1.0) / 2.0 * 2.0, 1.0);
	fragPos = gs_in[2].fragPos;
	Normal = gs_in[2].Normal;
	texCoord = gs_in[2].texCoord;
	EmitVertex();
	
	EndPrimitive();
};