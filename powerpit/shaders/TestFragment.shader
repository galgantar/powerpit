#version 330 core

struct Material
{
    sampler2D diffuseMap;
    sampler2D specularMap;
    float shininess;
};

in VS_OUT{
	vec3 Normal;
	vec3 fragPos;
	vec2 texCoord;
} fs_in;

uniform Material material;

out vec4 FragColor;

void main()
{
    FragColor = texture(material.diffuseMap, fs_in.texCoord);
    //FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}