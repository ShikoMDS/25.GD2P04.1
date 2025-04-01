/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2024 Media Design School

File Name : ReflectionFragmentShader.frag
Description : Fragment shader file for reflection model rendering (not implemented)
Author : Shikomisen (Ayoub Ahmad)
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#version 460 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec3 ReflectDir;

uniform sampler2D texture_diffuse1;
uniform samplerCube skybox;
uniform bool useTexture;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 textureColor = texture(texture_diffuse1, TexCoords).rgb;

    vec3 envColor = texture(skybox, ReflectDir).rgb;

    vec3 resultColor = mix(textureColor, envColor, 0.5);
    FragColor = vec4(resultColor, 1.0);
}
