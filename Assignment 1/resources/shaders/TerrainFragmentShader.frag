#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
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

uniform sampler2D terrainTextures[4]; // Grass, Dirt, Rock, Snow
uniform vec3 terrainColors[4];        // Color alternatives when textures aren't available
uniform float heightLevels[4];        // Height thresholds for each texture
uniform float blendFactor;            // Controls smoothness of transitions
uniform vec3 viewPos;                 // Camera position
uniform Material material;
uniform DirectionalLight directionalLight;
uniform bool useTextures;             // Whether to use textures or solid colors

// Function to calculate texture blend weights based on height
vec4 calculateBlendWeights(float height) {
    vec4 weights = vec4(0.0);
    
    // Define blend regions with smooth transitions
    float blendRange = blendFactor; // Controls transition width
    
    // Calculate weights for each texture layer based on height
    // Grass (lowest)
    weights.r = 1.0 - smoothstep(heightLevels[0], heightLevels[1], height - blendRange);
    
    // Dirt (low-mid)
    weights.g = smoothstep(heightLevels[0], heightLevels[1], height) * 
               (1.0 - smoothstep(heightLevels[1], heightLevels[2], height - blendRange));
    
    // Rock (mid-high)
    weights.b = smoothstep(heightLevels[1], heightLevels[2], height) * 
               (1.0 - smoothstep(heightLevels[2], heightLevels[3], height - blendRange));
    
    // Snow (highest)
    weights.a = smoothstep(heightLevels[2], heightLevels[3], height);
    
    // Normalize weights to ensure they sum to 1.0
    float totalWeight = weights.r + weights.g + weights.b + weights.a;
    if (totalWeight > 0.0) {
        weights /= totalWeight;
    } else {
        // Fallback: use lowest texture if no weights are assigned
        weights.r = 1.0;
    }
    
    return weights;
}

void main() {
    // Calculate the blend weights based on height
    vec4 blendWeights = calculateBlendWeights(Height);
    
    vec3 baseColor;
    
    // Decide whether to use textures or solid colors
    if (useTextures) {
        // Try to sample textures but with fallback mechanism
        vec4 grassColor = texture(terrainTextures[0], TexCoords * 20.0);
        vec4 dirtColor = texture(terrainTextures[1], TexCoords * 20.0);
        vec4 rockColor = texture(terrainTextures[2], TexCoords * 15.0);
        vec4 snowColor = texture(terrainTextures[3], TexCoords * 10.0);
        
        // Check if textures appear to be valid
        bool validTextures = 
            (grassColor.a > 0.0) && 
            (dirtColor.a > 0.0) && 
            (rockColor.a > 0.0) && 
            (snowColor.a > 0.0);
            
        if (validTextures) {
            // Blend textures based on weights
            vec4 textureColor = 
                blendWeights.r * grassColor +
                blendWeights.g * dirtColor +
                blendWeights.b * rockColor +
                blendWeights.a * snowColor;
                
            baseColor = textureColor.rgb;
        } else {
            // Fallback to solid colors if textures don't seem valid
            baseColor = 
                blendWeights.r * terrainColors[0] +
                blendWeights.g * terrainColors[1] +
                blendWeights.b * terrainColors[2] +
                blendWeights.a * terrainColors[3];
        }
    } else {
        // Use solid colors
        baseColor = 
            blendWeights.r * terrainColors[0] +
            blendWeights.g * terrainColors[1] +
            blendWeights.b * terrainColors[2] +
            blendWeights.a * terrainColors[3];
    }
    
    // Lighting calculations
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Directional light
    vec3 lightDir = normalize(-directionalLight.direction);
    
    // Ambient - reduced multiplier
    vec3 ambient = directionalLight.color * material.ambient * directionalLight.intensity * 0.5;

    // Diffuse - Use half-Lambert wrapping for softer lighting
    float NdotL = dot(norm, lightDir);
    float diff = max(NdotL * 0.5 + 0.5, 0.0) * 0.7; // Half-Lambert with reduced intensity
    vec3 diffuse = directionalLight.color * (diff * material.diffuse) * directionalLight.intensity;

    // Specular (modified Blinn-Phong) - with reduced intensity
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    spec *= 0.3; // Reduce specular intensity to prevent whitening
    vec3 specular = directionalLight.color * (spec * material.specular) * directionalLight.intensity;

    // Add ambient occlusion effect based on height to darken valleys
    float ao = mix(0.5, 1.0, smoothstep(0.0, 0.3, Height));
    ambient *= ao;
    diffuse *= ao;
    
    // Combine lighting with base color
    vec3 result = (ambient + diffuse) * baseColor + specular;
    
    // Additional effects
    
    // Darken valleys (based on height)
    float valleyFactor = smoothstep(0.0, 0.3, Height);
    result *= mix(0.8, 1.0, valleyFactor);
    
    // Add slight blue tint to snow at distance (fake atmospheric effect)
    float snowAmount = blendWeights.a;
    if (snowAmount > 0.0) {
        float distanceFactor = length(viewPos - FragPos) / 100.0;
        distanceFactor = clamp(distanceFactor, 0.0, 1.0);
        vec3 snowTint = mix(result, vec3(0.8, 0.9, 1.0), distanceFactor * 0.3 * snowAmount);
        result = mix(result, snowTint, snowAmount);
    }
    
    // Ensure colors are properly clamped and vibrant
    result = clamp(result, vec3(0.0), vec3(1.0));
    
    FragColor = vec4(result, 1.0);
}