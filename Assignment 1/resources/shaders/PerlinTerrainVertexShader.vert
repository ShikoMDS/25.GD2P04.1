#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out float Height;  // This value is critical for color determination

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    // Calculate world position
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // Transform normals to world space
    Normal = mat3(transpose(inverse(model))) * aNormal;
    
    // Pass texture coordinates
    TexCoords = aTexCoords;
    
    // Important: Calculate normalized height value for color determination
    // Use the raw Y coordinate before any model transformations
    // Assuming the heightmap range is approximately -1 to 1
    Height = (aPos.y + 1.0) * 0.5;  // Convert from [-1,1] to [0,1] range
    
    // Alternative calculation if needed:
    // If your terrain Y values are in a different range, adjust accordingly
    // float minHeight = -1.0;  // Minimum possible height in your heightmap
    // float maxHeight = 1.0;   // Maximum possible height in your heightmap
    // Height = (aPos.y - minHeight) / (maxHeight - minHeight);
    
    // Output position
    gl_Position = projection * view * vec4(FragPos, 1.0);
}