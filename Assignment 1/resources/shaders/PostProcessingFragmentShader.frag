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
    float t = time;
    
    // Sample the base scene color (you can also use your blur logic if desired)
    vec3 baseColor = texture(screenTexture, uv).rgb;
    
    // Parameters for the rain effect
    float dropSpeed = 2.0;   // Droplets fall faster
    int numDroplets = 300;   // More droplets for a continuous effect
    float dropletAccum = 0.0;

    // Loop over droplets
    for (int i = 0; i < numDroplets; i++) {
        // Each droplet gets a fixed random horizontal position and a random phase offset
        float randomX = fract(sin(float(i) * 12.9898) * 43758.5453);
        float phase = fract(cos(float(i) * 78.233) * 43758.5453);
        
        // Force droplet initial Y near the top (i.e., 1.0)
        // Then, subtract time * dropSpeed and add the random phase so each droplet falls at its own pace.
        // The fract() wraps the value so once it goes below 0, it reappears at the top.
        float dropletY = fract(1.0 - (t * dropSpeed + phase));
        
        // Form the droplet position
        vec2 dropletPos = vec2(randomX, dropletY);
        
        // Add horizontal sliding: a slight sinusoidal offset based on time and the droplet’s phase.
        float slideOffset = sin(t + phase * 6.2831) * 0.005;
        dropletPos.x += slideOffset;
        
        // Each droplet gets a random size between 0.01 and 0.03
        float dropSize = 0.01 + fract(sin(float(i) * 34.56)) * 0.02;
        
        // Compute distance from the current fragment to the droplet center, scaled for droplet size sensitivity.
        float dist = length(uv - dropletPos);
        
        // Use smoothstep to create a soft-edged droplet:
        // The intensity is 1.0 at the center and falls off to 0 at dropSize.
        float droplet = 1.0 - smoothstep(0.0, dropSize, dist);
        
        // Accumulate the droplet intensity; a multiplier reduces overall opacity.
        dropletAccum += droplet * 0.5;
    }
    
    // Clamp the final accumulated droplet effect to [0, 1].
    dropletAccum = clamp(dropletAccum, 0.0, 1.0);
    
    // Wet window tint: a subtle light-blue tint to simulate a rainy window.
    vec3 wetTint = vec3(0.7, 0.75, 0.85);
    // Blend the base scene color with the wet tint, modulated by the droplet intensity.
    vec3 finalColor = mix(baseColor, wetTint, dropletAccum * 0.5);
    
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