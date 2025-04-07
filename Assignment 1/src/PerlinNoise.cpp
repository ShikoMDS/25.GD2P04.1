/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2024 Media Design School

File Name : PerlinNoise.cpp
Description : Implementation of Perlin noise generator
**************************************************************************/

#include "PerlinNoise.h"
#include <iostream>
#include <algorithm>

// Define this macro before including the STB Image Write implementation
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

PerlinNoise::PerlinNoise(unsigned int seed) {
    // Initialize the permutation vector with the reference values
    p = {
        151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225,
        140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148,
        247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32,
        57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175,
        74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122,
        60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54,
        65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169,
        200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64,
        52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212,
        207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213,
        119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
        129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104,
        218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241,
        81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157,
        184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93,
        222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
    };

    // Duplicate the permutation vector
    p.insert(p.end(), p.begin(), p.end());

    // If a seed is provided, shuffle the permutation vector using the seed
    if (seed != 0) {
        std::mt19937 rng(seed);
        std::shuffle(p.begin(), p.begin() + 256, rng);
        std::copy(p.begin(), p.begin() + 256, p.begin() + 256);
    }
}

float PerlinNoise::fade(float t) {
    // Fade function as defined by Ken Perlin: 6t^5 - 15t^4 + 10t^3
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float PerlinNoise::lerp(float a, float b, float t) {
    // Linear interpolation
    return a + t * (b - a);
}

float PerlinNoise::grad(int hash, float x, float y, float z) {
    // Convert low 4 bits of hash into 12 gradient directions
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float PerlinNoise::noise(float x, float y) const {
    // 2D noise using 3D implementation with z=0
    return noise(x, y, 0.0f);
}

float PerlinNoise::noise(float x, float y, float z) const {
    // Find unit cube that contains point
    int X = static_cast<int>(std::floor(x)) & 255;
    int Y = static_cast<int>(std::floor(y)) & 255;
    int Z = static_cast<int>(std::floor(z)) & 255;

    // Find relative x, y, z of point in cube
    x -= std::floor(x);
    y -= std::floor(y);
    z -= std::floor(z);

    // Compute fade curves for each of x, y, z
    float u = fade(x);
    float v = fade(y);
    float w = fade(z);

    // Hash coordinates of the 8 cube corners
    int A = p[X % 256] + Y;
    int AA = p[(A % 256)];
    int AB = p[(A + 1) % 256];
    int B = p[X + 1] + Y;
    int BA = p[B] + Z;
    int BB = p[B + 1] + Z;

    // Add blended results from 8 corners of cube
    return lerp(
        lerp(
            lerp(grad(p[AA], x, y, z), grad(p[BA], x - 1, y, z), u),
            lerp(grad(p[AB], x, y - 1, z), grad(p[BB], x - 1, y - 1, z), u),
            v
        ),
        lerp(
            lerp(grad(p[AA + 1], x, y, z - 1), grad(p[BA + 1], x - 1, y, z - 1), u),
            lerp(grad(p[AB + 1], x, y - 1, z - 1), grad(p[BB + 1], x - 1, y - 1, z - 1), u),
            v
        ),
        w
    );
}

float PerlinNoise::fractalNoise(float x, float y, int octaves, float persistence) const {
    float total = 0;
    float frequency = 1;
    float amplitude = 1;
    float maxValue = 0;  // Used for normalizing result to 0.0 - 1.0

    for (int i = 0; i < octaves; i++) {
        total += noise(x * frequency, y * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2;
    }

    return total / maxValue;
}

std::vector<float> PerlinNoise::generateNoiseMap(int width, int height, float scale, int octaves, float persistence, float lacunarity, glm::vec2 offset) {
    std::vector<float> noiseMap(width * height);

    // Prevent division by zero
    if (scale <= 0) scale = 0.0001f;

    float maxNoiseHeight = std::numeric_limits<float>::min();
    float minNoiseHeight = std::numeric_limits<float>::max();

    // Generate noise samples
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float amplitude = 1;
            float frequency = 1;
            float noiseHeight = 0;

            // Compute fractal Brownian motion
            for (int i = 0; i < octaves; i++) {
                float sampleX = (x - width / 2 + offset.x) / scale * frequency;
                float sampleY = (y - height / 2 + offset.y) / scale * frequency;

                float noiseValue = noise(sampleX, sampleY) * 2 - 1; // Range -1 to 1
                noiseHeight += noiseValue * amplitude;

                amplitude *= persistence;
                frequency *= lacunarity;
            }

            // Track min and max for normalization
            if (noiseHeight > maxNoiseHeight) maxNoiseHeight = noiseHeight;
            if (noiseHeight < minNoiseHeight) minNoiseHeight = noiseHeight;

            noiseMap[y * width + x] = noiseHeight;
        }
    }

    // Normalize the noise map to [0, 1]
    if (maxNoiseHeight > minNoiseHeight) {
        for (int i = 0; i < width * height; i++) {
            noiseMap[i] = (noiseMap[i] - minNoiseHeight) / (maxNoiseHeight - minNoiseHeight);
        }
    }

    return noiseMap;
}

bool PerlinNoise::saveAsRaw(const std::vector<float>& noiseMap, int width, int height, const std::string& filename) {
    try {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for writing: " << filename << std::endl;
            return false;
        }

        // Convert float values (0-1) to bytes (0-255)
        std::vector<unsigned char> byteData(width * height);
        for (int i = 0; i < width * height; i++) {
            byteData[i] = static_cast<unsigned char>(noiseMap[i] * 255.0f);
        }

        // Write raw bytes to file
        file.write(reinterpret_cast<const char*>(byteData.data()), byteData.size());
        file.close();

        std::cout << "Successfully saved RAW file: " << filename << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception while saving RAW file: " << e.what() << std::endl;
        return false;
    }
}

glm::vec3 PerlinNoise::applyColorGradient(float noiseValue, const std::vector<glm::vec3>& colorGradient) const {
    // Ensure there are at least 2 colors in the gradient
    if (colorGradient.size() < 2) {
        return glm::vec3(noiseValue);
    }

    // Map the noise value to the gradient
    float gradientPos = noiseValue * (colorGradient.size() - 1);
    int index = static_cast<int>(gradientPos);
    float t = gradientPos - index; // Fractional part for interpolation

    // Ensure index is within bounds
    index = std::max(0, std::min(static_cast<int>(colorGradient.size()) - 2, index));

    // Interpolate between the two colors
    glm::vec3 color1 = colorGradient[index];
    glm::vec3 color2 = colorGradient[index + 1];

    if (colorGradient.empty()) {
        return glm::vec3(noiseValue);
    }
    if (colorGradient.size() == 1) {
        return colorGradient[0];
    }

    return color1 * (1 - t) + color2 * t;
}

bool PerlinNoise::saveAsJpg(const std::vector<float>& noiseMap, int width, int height, const std::string& filename, const std::vector<glm::vec3>& colorGradient) {
    try {
        // Convert noise values to RGB using the color gradient
        std::vector<unsigned char> imageData(width * height * 3);

        for (int i = 0; i < width * height; i++) {
            glm::vec3 color = applyColorGradient(noiseMap[i], colorGradient);

            // Convert floating point color (0-1) to byte color (0-255)
            imageData[i * 3] = static_cast<unsigned char>(color.r * 255.0f);     // R
            imageData[i * 3 + 1] = static_cast<unsigned char>(color.g * 255.0f); // G
            imageData[i * 3 + 2] = static_cast<unsigned char>(color.b * 255.0f); // B
        }

        // Save the image
        int result = stbi_write_jpg(filename.c_str(), width, height, 3, imageData.data(), 100); // 100 is quality (0-100)

        if (result == 0) {
            std::cerr << "Failed to write JPG file: " << filename << std::endl;
            return false;
        }

        std::cout << "Successfully saved JPG file: " << filename << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception while saving JPG file: " << e.what() << std::endl;
        return false;
    }
}

GLuint PerlinNoise::createNoiseTexture(const std::vector<float>& noiseMap, int width, int height, const std::vector<glm::vec3>& colorGradient) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Convert noise values to RGB using the color gradient
    std::vector<unsigned char> textureData(width * height * 3);

    for (int i = 0; i < width * height; i++) {
        glm::vec3 color = applyColorGradient(noiseMap[i], colorGradient);

        // Convert floating point color (0-1) to byte color (0-255)
        textureData[i * 3] = static_cast<unsigned char>(color.r * 255.0f);     // R
        textureData[i * 3 + 1] = static_cast<unsigned char>(color.g * 255.0f); // G
        textureData[i * 3 + 2] = static_cast<unsigned char>(color.b * 255.0f); // B
    }

    // Create the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData.data());

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    return textureID;
}