#version 330 core

out vec4 FragColor;

struct Material
{
    sampler2D diffuseMap;
    sampler2D specularMap;
    float shininess;
};

struct DirectionalLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant_val;
    float linear_val;
    float quadratic_val;
};

in vec3 Normal;
in vec3 fragPos;
in vec2 texCoord;

uniform vec3 viewPos;
uniform Material material;
uniform DirectionalLight dirLight;
uniform PointLight pointLights[4];

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * texture(material.diffuseMap, texCoord).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuseMap, texCoord).rgb;
    vec3 specular = light.specular * spec * texture(material.specularMap, texCoord).rgb;
    
    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.quadratic_val * distance * distance + light.linear_val * distance + light.constant_val);

    vec3 ambient = light.ambient * texture(material.diffuseMap, texCoord).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuseMap, texCoord).rgb;
    vec3 specular = light.specular * spec * texture(material.specularMap, texCoord).rgb;

    return (ambient + diffuse) * attenuation;
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - fragPos);

    vec3 finalFragColor = CalcDirectionalLight(dirLight, norm, viewDir);

    for (int i = 0; i < 4; ++i)
        finalFragColor += CalcPointLight(pointLights[i], norm, viewDir);

    FragColor = vec4(finalFragColor, 1.0);
}