/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2025 Media Design School

File Name : PerlinNoise.h
Description : Perlin noise generation for terrain and effects
Author : Shikomisen (Ayoub Ahmad)
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#pragma once

#include <vector>
#include <random>
#include <cmath>
#include <ctime>
#include <string>
#include <glew.h>
#include <glm.hpp>
#include <fstream>


class PerlinNoise
{
public:
	explicit PerlinNoise(unsigned int Seed = static_cast<unsigned int>(std::time(nullptr)));

	// Generate 2D Perlin noise
	[[nodiscard]] float noise(float X, float Y) const;
	// Generate 3D Perlin noise 
	[[nodiscard]] float noise(float X, float Y, float Z) const;
	[[nodiscard]] float fractalNoise(float X, float Y, int Octaves, float Persistence) const;
	std::vector<float> generateNoiseMap(int Width, int Height, float Scale, int Octaves, float Persistence,
	                                    float Lacunarity, glm::vec2 Offset = glm::vec2(0, 0)) const;

	static bool saveAsRaw(const std::vector<float>& NoiseMap, int Width, int Height, const std::string& Filename);

	static bool saveAsJpg(const std::vector<float>& NoiseMap, int Width, int Height, const std::string& Filename,
	                      const std::vector<glm::vec3>& ColourGradient);

	static GLuint createNoiseTexture(const std::vector<float>& NoiseMap, int Width, int Height,
	                                 const std::vector<glm::vec3>& ColourGradient);

	[[nodiscard]] static glm::vec3 applyColourGradient(float NoiseValue, const std::vector<glm::vec3>& ColourGradient);

private:
	std::vector<int> PvPermutation;

	static float fade(float T);
	static float lerp(float A, float B, float T);
	static float grad(int Hash, float X, float Y, float Z);
};
