#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in float Height;  // Height should be in 0-1 range

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

uniform vec3 viewPos;
uniform Material material;
uniform DirectionalLight directionalLight;
uniform vec3 terrainColors[4];    // Black, Red, Orange, Yellow/White
uniform float heightLevels[4];    // Height thresholds for each color
uniform float blendFactor;        // Controls smoothness of transitions

void main() {
    // Use height directly for color calculation
    vec3 baseColor;
    
    // Get height for color selection (ensure it's in 0-1 range)
    float h = clamp(Height, 0.0, 1.0);
    
    // Simple 4-color gradient with sharp transitions for debugging
    // if (h < heightLevels[1]) {
    //     baseColor = terrainColors[0]; // Black for lowest
    // } else if (h < heightLevels[2]) {
    //     baseColor = terrainColors[1]; // Red for low-mid
    // } else if (h < heightLevels[3]) {
    //     baseColor = terrainColors[2]; // Orange for mid-high
    // } else {
    //     baseColor = terrainColors[3]; // Yellow/White for highest
    // }
    
    // Smooth blending between all four colors
    if (h < heightLevels[1]) {
        float t = smoothstep(heightLevels[0], heightLevels[1], h);
        baseColor = mix(terrainColors[0], terrainColors[1], t);
    } 
    else if (h < heightLevels[2]) {
        float t = smoothstep(heightLevels[1], heightLevels[2], h);
        baseColor = mix(terrainColors[1], terrainColors[2], t);
    } 
    else {
        float t = smoothstep(heightLevels[2], heightLevels[3], h);
        baseColor = mix(terrainColors[2], terrainColors[3], t);
    }
    
    // Lighting calculations
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lightDir = normalize(-directionalLight.direction);
    
    // Ambient component
    vec3 ambient = directionalLight.color * material.ambient;
    
    // Diffuse component (using half-Lambert for softer look)
    float NdotL = dot(norm, lightDir);
    float diff = max(NdotL * 0.5 + 0.5, 0.0) * 0.7;
    vec3 diffuse = directionalLight.color * material.diffuse * diff * directionalLight.intensity;
    
    // Specular component
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = directionalLight.color * material.specular * spec * directionalLight.intensity;
    
    // Combine lighting with base color
    vec3 result = (ambient + diffuse) * baseColor + specular;
    
    // Add edge highlighting for better visual definition
    float rim = 1.0 - max(dot(viewDir, norm), 0.0);
    rim = smoothstep(0.6, 0.9, rim);
    vec3 rimColor = terrainColors[3] * rim * 0.3;
    result += rimColor;
    
    // Final output
    FragColor = vec4(result, 1.0);
}