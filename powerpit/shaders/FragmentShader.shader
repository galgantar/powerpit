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

struct SpotLight
{
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float innerCutoff;
    float outerCutoff;

    float constant_val;
    float linear_val;
    float quadratic_val;
};

in VS_OUT{
    vec3 Normal;
    vec3 fragPos;
    vec2 texCoord;
} fs_in;


uniform vec3 viewPos;
uniform Material material;
uniform DirectionalLight dirLight;
//uniform SpotLight spotLight;
uniform PointLight pointLights[4];

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * texture(material.diffuseMap, fs_in.texCoord).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuseMap, fs_in.texCoord).rgb;
    vec3 specular = light.specular * spec * texture(material.specularMap, fs_in.texCoord).rgb;
    
    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fs_in.fragPos);
    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // attenuation
    float distance = length(light.position - fs_in.fragPos);
    float attenuation = 1.0 / (light.quadratic_val * distance * distance + light.linear_val * distance + light.constant_val);

    vec3 ambient = light.ambient * texture(material.diffuseMap, fs_in.texCoord).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuseMap, fs_in.texCoord).rgb;
    vec3 specular = light.specular * spec * texture(material.specularMap, fs_in.texCoord).rgb;

    return (ambient + diffuse + specular) * attenuation;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fs_in.fragPos);

    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * texture(material.diffuseMap, fs_in.texCoord).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuseMap, fs_in.texCoord).rgb;
    vec3 specular = light.specular * spec * texture(material.specularMap, fs_in.texCoord).rgb;
    
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.innerCutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

    return (specular + diffuse) * intensity + ambient;
}

void main()
{
    vec3 norm = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.fragPos);

    vec3 finalFragColor = vec3(0.0, 0.0, 0.0);
    
    finalFragColor = CalcDirectionalLight(dirLight, norm, viewDir);
 
    //finalFragColor += CalcSpotLight(spotLight, norm, viewDir);

    for (int i = 0; i < 4; ++i)
        finalFragColor += CalcPointLight(pointLights[i], norm, viewDir);

    //FragColor = vec4(finalFragColor, 1.0);
    FragColor = vec4(texture(material.diffuseMap, fs_in.texCoord).rgb, 1.0);
}