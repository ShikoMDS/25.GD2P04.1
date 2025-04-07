#version 460 core
out vec4 FragColor;

// These must match exactly with the vertex shader outputs
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;  // Make sure this is declared exactly the same as in vertex shader
in float Height;

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
uniform vec3 terrainColors[4];    // Fire gradient colors for height-based coloring
uniform float heightLevels[4];    // Height thresholds for each color
uniform float blendFactor;        // Controls smoothness of transitions

// Function to calculate color blending based on height
vec3 calculateHeightBasedColor(float height) {
    // Interpolate between colors based on height
    vec3 color;
    
    // Apply smooth transitions between color bands
    if (height < heightLevels[1]) {
        // Interpolate between first two colors
        float t = smoothstep(heightLevels[0], heightLevels[1], height);
        color = mix(terrainColors[0], terrainColors[1], t);
    } 
    else if (height < heightLevels[2]) {
        // Interpolate between second and third colors
        float t = smoothstep(heightLevels[1], heightLevels[2], height);
        color = mix(terrainColors[1], terrainColors[2], t);
    } 
    else {
        // Interpolate between third and fourth colors
        float t = smoothstep(heightLevels[2], heightLevels[3], height);
        color = mix(terrainColors[2], terrainColors[3], t);
    }
    
    return color;
}

void main() {
    // Get base color from height
    vec3 baseColor = calculateHeightBasedColor(Height);
    
    // Normalize vectors for lighting calculations
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lightDir = normalize(-directionalLight.direction);
    
    // Ambient component
    vec3 ambient = directionalLight.color * material.ambient * 0.3;
    
    // Diffuse component with half-lambert for softer appearance
    float diff = max(dot(norm, lightDir) * 0.5 + 0.5, 0.0);
    vec3 diffuse = directionalLight.color * material.diffuse * diff * directionalLight.intensity;
    
    // Specular component
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = directionalLight.color * material.specular * spec * directionalLight.intensity;
    
    // Combine lighting with base color
    vec3 result = (ambient + diffuse) * baseColor + specular;
    
    // Add edge highlight effect
    float edge = 1.0 - max(dot(viewDir, norm), 0.0);
    edge = smoothstep(0.4, 0.8, edge);
    vec3 edgeColor = mix(terrainColors[2], terrainColors[3], 0.5) * edge * 0.3;
    result += edgeColor;
    
    // Output final color
    FragColor = vec4(result, 1.0);
}