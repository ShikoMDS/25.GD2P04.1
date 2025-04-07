#version 460 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{
    // Simply sample the texture at the provided texture coordinates
    FragColor = texture(texture1, TexCoords);
    
    // Add a subtle border effect
    vec2 border = smoothstep(0.0, 0.02, TexCoords) * smoothstep(0.0, 0.02, vec2(1.0) - TexCoords);
    float borderFactor = border.x * border.y;
    
    // Darken the edges
    FragColor.rgb *= borderFactor;
}