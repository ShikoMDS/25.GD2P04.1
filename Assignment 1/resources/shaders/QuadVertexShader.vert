#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;  // Changed from aTexCoords to aNormal to match Quad.cpp
layout (location = 2) in vec2 aTexCoords;  // Changed to location 2 to match Quad.cpp

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}