/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2025 Media Design School

File Name : AnimationVertexShader.vert
Description : Animation vertex shader for Scene 3 requirements
Author : Ayoub Ahmad
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

void main()
{
    // Pass texture coordinates to fragment shader
    TexCoords = aTexCoords;
    
    // Apply simple wave animation based on time
    vec3 position = aPos;
    position.z = sin(position.x * 5.0 + time * 2.0) * 0.1;
    
    gl_Position = projection * view * model * vec4(position, 1.0);
}