/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2024 Media Design School

File Name : PerlinNoise.h
Description : Perlin noise generation for terrain and effects
**************************************************************************/

#pragma once

#include <vector>
#include <random>
#include <cmath>
#include <ctime>
#include <string>
#include <glew.h>
#include <glm.hpp>  // Added GLM include
#include <fstream>


class PerlinNoise {
public:
    // Constructor with optional seed (default uses current time)
    PerlinNoise(unsigned int seed = (unsigned int)std::time(nullptr));

    // Generate 2D Perlin noise
    float noise(float x, float y) const;

    // Generate 3D Perlin noise (added to match implementation)
    float noise(float x, float y, float z) const;

    // Generate 2D Perlin noise with multiple octaves (fractal)
    float fractalNoise(float x, float y, int octaves, float persistence) const;

    // Generate a noise map of specified dimensions
    std::vector<float> generateNoiseMap(int width, int height, float scale, int octaves, float persistence, float lacunarity, glm::vec2 offset = glm::vec2(0, 0));

    // Save noise data as RAW file
    bool saveAsRaw(const std::vector<float>& noiseMap, int width, int height, const std::string& filename);

    // Save noise data as JPG file with color gradient
    bool saveAsJpg(const std::vector<float>& noiseMap, int width, int height, const std::string& filename, const std::vector<glm::vec3>& colorGradient);

    // Create a texture from noise data with color gradient
    GLuint createNoiseTexture(const std::vector<float>& noiseMap, int width, int height, const std::vector<glm::vec3>& colorGradient);

    // Apply color gradient to a noise value
    glm::vec3 applyColorGradient(float noiseValue, const std::vector<glm::vec3>& colorGradient) const;

private:
    std::vector<int> p; // Permutation table

    // Helper functions made static to avoid non-static member function calls
    static float fade(float t);
    static float lerp(float a, float b, float t);
    static float grad(int hash, float x, float y, float z);
};