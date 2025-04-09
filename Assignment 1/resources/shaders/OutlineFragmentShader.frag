/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2025 Media Design School

File Name : OutlineFragmentShader.frag
Description : Outline Fragment shader for object stenciling
Author : Shikomisen (Ayoub Ahmad)
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#version 460 core

out vec4 FragColor;

uniform vec3 outlineColor;

void main() 
{
    FragColor = vec4(outlineColor, 1.0);
}
