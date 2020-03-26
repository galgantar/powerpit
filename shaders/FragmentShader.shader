#version 330 core

in vec2 texCoord;

uniform sampler2D smiley;
uniform sampler2D box;

out vec4 FragColor;

void main()
{
    FragColor = mix(texture(box, texCoord), texture(smiley, texCoord), 0.2);
};