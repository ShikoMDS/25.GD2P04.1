#version 460 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{
    // Sample texture at texture coordinates
    FragColor = texture(texture1, TexCoords);
}