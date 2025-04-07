#version 460 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D texture1;
uniform float time;

void main()
{
    // Create flowing distortion effect
    vec2 texCoord = TexCoords;
    
    // Add ripple/wave effect
    texCoord.x += sin(texCoord.y * 20.0 + time * 2.0) * 0.02;
    texCoord.y += cos(texCoord.x * 20.0 + time * 2.0) * 0.02;
    
    // Add secondary flow effect
    float flowSpeed = time * 0.2;
    texCoord.y -= flowSpeed;
    
    // Sample the texture with distorted coordinates
    vec4 texColor = texture(texture1, texCoord);
    
    // Add pulsing intensity effect
    float pulse = (sin(time * 3.0) + 1.0) * 0.1 + 0.9;
    texColor.rgb *= pulse;
    
    // Add color shifting effect over time
    float colorShift = sin(time * 0.5) * 0.2 + 0.8;
    texColor.r *= colorShift;
    texColor.b *= (1.0 / colorShift);
    
    FragColor = texColor;
}