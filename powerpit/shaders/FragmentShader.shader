#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 fragPos;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

vec3 CalculateAmbient()
{
    return lightColor * vec3(0.1f, 0.1f, 0.1f);
};

vec3 CalculateDiffuse()
{
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDirection), 0.0);
    return lightColor * diff;
}

vec3 CalculateSpecular()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 lightDir = normalize(fragPos - lightPos);
    vec3 reflectDir = reflect(lightDir, norm);

    float specularStrength = 0.5;
    int shinnines = 50;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shinnines);
    return lightColor * spec * specularStrength;
}

void main()
{
    vec3 finalLightColor = CalculateAmbient() + CalculateDiffuse() + CalculateSpecular();

    FragColor = vec4(objectColor * finalLightColor, 1.0);
};