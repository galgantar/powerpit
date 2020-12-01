#version 330 core

out vec4 FragColor;

in vec3 fragPos;
in vec3 Normal;

uniform samplerCube skybox;
uniform vec3 cameraPos;

void main()
{
	float ratio = 1.0 / 1.52; // air to glass
	vec3 I = normalize(fragPos - cameraPos);
	vec3 R = refract(I, normalize(Normal), ratio);
	FragColor = vec4(texture(skybox, R).rgb, 1.0);

}