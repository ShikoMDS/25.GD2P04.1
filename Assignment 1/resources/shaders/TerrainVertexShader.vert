/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2025 Media Design School

File Name : TerrainVertexShader.vert
Description : Terrain vertex shader for specified requirements
Author : Ayoub Ahmad
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out float Height;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() 
{
    // Pass the vertex position in world space
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // Transform normals to world space (excluding scaling and translation)
    Normal = mat3(transpose(inverse(model))) * aNormal;
    
    // Pass texture coordinates
    TexCoords = aTexCoords;
    
    // Calculate normalized height
    // This is the normalized height (0-1) used for texture blending
    // Adjust this calculation to account for the terrain's position at Y=2.5
    // Normalize based on the terrain's actual height range (not including its Y position)
    float rawHeight = FragPos.y;
    Height = (rawHeight - 2.5f) / 20.0f; // Normalize to 0-1 range, adjusting for Y=2.5 position
    Height = clamp(Height, 0.0f, 1.0f);
    
    // Set final position in clip space
    gl_Position = projection * view * vec4(FragPos, 1.0);
}