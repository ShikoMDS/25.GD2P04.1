#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

// Make sure these output variables match exactly with the fragment shader inputs
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords; // This must be declared identically in both shaders
out float Height; // Height for color blending based on elevation

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    // Calculate world position
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // Transform normals to world space (excluding scaling)
    Normal = mat3(transpose(inverse(model))) * aNormal;
    
    // Pass texture coordinates
    TexCoords = aTexCoords;
    
    // Calculate normalized height value for Perlin visualization
    // Using raw Y coordinate before model transformation (original heightmap data)
    // Normalize from assumed [-1, 1] range to [0, 1] for color blending
    Height = (aPos.y + 1.0) * 0.5;
    
    // Final position
    gl_Position = projection * view * vec4(FragPos, 1.0);
}