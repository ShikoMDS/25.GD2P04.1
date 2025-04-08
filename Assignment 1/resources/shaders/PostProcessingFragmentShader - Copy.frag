#version 460 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform int effect; // 0 = normal, 1 = inversion, 2 = grayscale, 3 = rain, 4 = custom

// Time for animated effects
uniform float time;

// Normal rendering (no effect)
vec4 normal() {
    return texture(screenTexture, TexCoords);
}

// Color inversion effect
vec4 inversion() {
    vec4 texColor = texture(screenTexture, TexCoords);
    return vec4(1.0 - texColor.rgb, texColor.a);
}

// Grayscale effect using luminosity method
vec4 grayscale() {
    vec4 texColor = texture(screenTexture, TexCoords);
    float luminosity = 0.299 * texColor.r + 0.587 * texColor.g + 0.114 * texColor.b;
    return vec4(luminosity, luminosity, luminosity, texColor.a);
}

vec4 rain() {
    vec2 uv = TexCoords;
    float iTime = time; // Use full time instead of halving
    // Sample the base scene
    vec3 baseColor = texture(screenTexture, uv).rgb;
    
    // Rain droplet color
    vec3 dropColor = vec3(0.2, 0.4, 0.8);
    
    // Keep track of droplet accumulation
    float droplets = 0.0;
    
    // Speed at which droplets fall (increase for faster rain)
    float dropSpeed = 1.5;
    
    // Increase the droplet count for denser rain
    int numDroplets = 300;
    
    for (int i = 0; i < numDroplets; i++) {
        // Randomly position droplets throughout full screen height [0..1], not just top half
        float initY = fract(cos(float(i) * 76.543) * 234.567); // Range [0..1]
        
        // Droplet’s current Y falls over time with wrapping
        float currentY = fract(initY - iTime * dropSpeed);
        
        // Compute X using a pseudo-random function
        float currentX = fract(sin(float(i) * 54.321) * 123.456);
        vec2 dropletPos = vec2(currentX, currentY);
        
        // Distance from fragment to droplet center
        float dist = length(uv - dropletPos);
        
        // Larger droplet range for more coverage
        float dropSize = 0.01 + fract(sin(float(i) * 12.34)) * 0.01; // Range ~ [0.01..0.02]
        
        // Create a droplet shape (1 at center, 0 at dropSize)
        float droplet = 1.0 - smoothstep(0.0, dropSize, dist);
        
        // Accumulate droplet intensity
        droplets += droplet;
    }
    
    // Over-saturate and then clamp
    // (If many droplets overlap, we get a value > 1, which we clamp to 2 before final blend)
    droplets = clamp(droplets, 0.0, 2.0);
    
    // Convert that droplet value into a [0..1] range for blending
    float finalMask = min(1.0, droplets);
    
    // Blend base color with droplet color (up to 70% droplet intensity)
    vec3 finalColor = mix(baseColor, dropColor, finalMask * 0.7);
    
    return vec4(finalColor, 1.0);
}

// Custom effect: CRT TV screen inspired by ShaderToy
vec4 crtScreen() {
    vec2 uv = TexCoords;
    
    // CRT curvature
    float curvature = 2.5;
    vec2 curvedUV = uv * 2.0 - 1.0; // Convert to -1 to 1 range
    vec2 offset = abs(curvedUV.yx) / vec2(curvature);
    curvedUV = curvedUV + curvedUV * offset * offset;
    curvedUV = curvedUV * 0.5 + 0.5; // Convert back to 0-1 range
    
    // If outside the curved screen, return black
    if (curvedUV.x < 0.0 || curvedUV.x > 1.0 || curvedUV.y < 0.0 || curvedUV.y > 1.0) {
        return vec4(0.0, 0.0, 0.0, 1.0);
    }
    
    // Scanlines
    float scanlineIntensity = 0.125;
    float scanline = sin(curvedUV.y * 800.0) * scanlineIntensity;
    
    // RGB shift (chromatic aberration)
    float rgbShiftAmount = 0.003;
    float redShift = texture(screenTexture, vec2(curvedUV.x + rgbShiftAmount, curvedUV.y)).r;
    float greenShift = texture(screenTexture, curvedUV).g;
    float blueShift = texture(screenTexture, vec2(curvedUV.x - rgbShiftAmount, curvedUV.y)).b;
    
    // Vignette effect
    float vignetteAmount = 0.2;
    float vignette = 1.0 - dot(curvedUV - 0.5, (curvedUV - 0.5) * 2.0);
    vignette = pow(vignette, 2.0) * vignetteAmount;
    
    // Noise/static
    float noiseIntensity = 0.05;
    float staticNoise = fract(sin(dot(curvedUV, vec2(12.9898, 78.233) * time) * 43758.5453));
    
    // Flicker effect (vertical hold)
    float flickerAmount = 0.03;
    float flicker = sin(time * 10.0) * flickerAmount;
    
    // Combine effects
    vec3 crtColor = vec3(redShift, greenShift, blueShift);
    crtColor -= scanline;
    crtColor += staticNoise * noiseIntensity;
    crtColor *= (1.0 - vignette);
    crtColor += flicker;
    
    return vec4(crtColor, 1.0);
}

void main() {
    switch(effect) {
        case 1:
            FragColor = inversion();
            break;
        case 2:
            FragColor = grayscale();
            break;
        case 3:
            FragColor = rain();
            break;
        case 4:
            FragColor = crtScreen();
            break;
        default:
            FragColor = normal();
            break;
    }
}