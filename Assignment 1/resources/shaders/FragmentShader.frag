#version 460 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

struct Material {
    sampler2D diffuseMap;  // Diffuse texture sampler (must be bound to unit 0)
    vec3 ambient;          // Ambient color (if no texture)
    vec3 diffuse;          // Diffuse color (if no texture)
    vec3 specular;
    float shininess;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
};

struct DirectionalLight {
    vec3 direction;
    vec3 color;
    float ambientStrength;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
};

uniform Material material;
uniform PointLight pointLights[2];
uniform DirectionalLight directionalLight;
uniform SpotLight spotLight;
uniform vec3 viewPos;
uniform bool useTexture; 
uniform vec3 solidColor; // (Not used in lighting calculations)

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 baseColor)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambientStrength * light.color * baseColor;
    vec3 diffuse = light.color * diff * baseColor;
    vec3 specular = light.color * spec * material.specular;

    return ambient + diffuse + specular;
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 baseColor)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient = light.color * baseColor;
    vec3 diffuse = light.color * diff * baseColor;
    vec3 specular = light.color * spec * material.specular;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 baseColor)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient = light.color * baseColor;
    vec3 diffuse = light.color * diff * baseColor;
    vec3 specular = light.color * spec * material.specular;

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return ambient + diffuse + specular;
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Choose the base color: if useTexture is true, sample from the texture; otherwise, use the material diffuse color.
    vec3 baseColor = useTexture ? texture(material.diffuseMap, TexCoords).rgb : material.diffuse;

    vec3 result = CalculateDirectionalLight(directionalLight, norm, viewDir, baseColor);
    for (int i = 0; i < 2; i++) {
        result += CalculatePointLight(pointLights[i], norm, FragPos, viewDir, baseColor);
    }
    result += CalculateSpotLight(spotLight, norm, FragPos, viewDir, baseColor);

    FragColor = vec4(result, 1.0);
}
