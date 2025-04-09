/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2025 Media Design School

File Name : PerlinNoise.cpp
Description : Implementation of Perlin noise generator
Author : Shikomisen (Ayoub Ahmad)
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#include "PerlinNoise.h"
#include <iostream>
#include <algorithm>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

PerlinNoise::PerlinNoise(const unsigned int Seed)
{
	// Initialize permutation vector with reference values
	PvPermutation = {
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
	PvPermutation.insert(PvPermutation.end(), PvPermutation.begin(), PvPermutation.end());

	// If seed is provided, shuffle the permutation vector using the seed
	if (Seed != 0)
	{
		std::mt19937 Rng(Seed);
		std::shuffle(PvPermutation.begin(), PvPermutation.begin() + 256, Rng);
		std::copy_n(PvPermutation.begin(), 256, PvPermutation.begin() + 256);
	}
}

float PerlinNoise::fade(const float T)
{
	// Fade function as defined by Ken Perlin: 6t^5 - 15t^4 + 10t^3
	return T * T * T * (T * (T * 6 - 15) + 10);
}

float PerlinNoise::lerp(const float A, const float B, const float T)
{
	// Linear interpolation
	return A + T * (B - A);
}

float PerlinNoise::grad(const int Hash, const float X, const float Y, const float Z)
{
	// Convert low 4 bits of hash into 12 gradient directions
	const int H = Hash & 15;
	const float U = H < 8 ? X : Y;
	const float V = H < 4 ? Y : H == 12 || H == 14 ? X : Z;
	return ((H & 1) == 0 ? U : -U) + ((H & 2) == 0 ? V : -V);
}

float PerlinNoise::noise(const float X, const float Y) const
{
	// 2D noise using 3D implementation with z=0
	return noise(X, Y, 0.0f);
}

float PerlinNoise::noise(float X, float Y, float Z) const
{
	// Find unit cube that contains point
	const int CubeX = static_cast<int>(std::floor(X)) & 255;
	const int CubeY = static_cast<int>(std::floor(Y)) & 255;
	const int CubeZ = static_cast<int>(std::floor(Z)) & 255;

	// Find relative x, y, z of point in cube
	X -= std::floor(X);
	Y -= std::floor(Y);
	Z -= std::floor(Z);

	// Compute fade curves for each of x, y, z
	const float U = fade(X);
	const float V = fade(Y);
	const float W = fade(Z);

	// Hash coordinates of 8 cube corners
	const int A = PvPermutation[CubeX % 256] + CubeY;
	const int Aa = PvPermutation[(A % 256)];
	const int Ab = PvPermutation[(A + 1) % 256];
	const int B = PvPermutation[CubeX + 1] + CubeY;
	const int Ba = PvPermutation[B] + CubeZ;
	const int Bb = PvPermutation[B + 1] + CubeZ;

	// Add blended results from 8 corners of cube
	return lerp(
		lerp(
			lerp(grad(PvPermutation[Aa], X, Y, Z), grad(PvPermutation[Ba], X - 1, Y, Z), U),
			lerp(grad(PvPermutation[Ab], X, Y - 1, Z), grad(PvPermutation[Bb], X - 1, Y - 1, Z), U),
			V
		),
		lerp(
			lerp(grad(PvPermutation[Aa + 1], X, Y, Z - 1), grad(PvPermutation[Ba + 1], X - 1, Y, Z - 1), U),
			lerp(grad(PvPermutation[Ab + 1], X, Y - 1, Z - 1), grad(PvPermutation[Bb + 1], X - 1, Y - 1, Z - 1), U),
			V
		),
		W
	);
}

float PerlinNoise::fractalNoise(const float X, const float Y, const int Octaves, const float Persistence) const
{
	float Total = 0;
	float Frequency = 1;
	float Amplitude = 1;
	float MaxValue = 0; // Normalizing result to 0.0 - 1.0

	for (int I = 0; I < Octaves; I++)
	{
		Total += noise(X * Frequency, Y * Frequency) * Amplitude;
		MaxValue += Amplitude;
		Amplitude *= Persistence;
		Frequency *= 2;
	}

	return Total / MaxValue;
}

std::vector<float> PerlinNoise::generateNoiseMap(const int Width, const int Height, float Scale, const int Octaves,
                                                 const float Persistence, const float Lacunarity,
                                                 const glm::vec2 Offset) const
{
	std::vector<float> NoiseMap(static_cast<size_t>(Width) * Height);

	// Prevent division by zero
	if (Scale <= 0) Scale = 0.0001f;

	float MaxNoiseHeight = std::numeric_limits<float>::min();
	float MinNoiseHeight = std::numeric_limits<float>::max();

	// Generate noise samples
	for (int Y = 0; Y < Height; Y++)
	{
		for (int X = 0; X < Width; X++)
		{
			float Amplitude = 1;
			float Frequency = 1;
			float NoiseHeight = 0;

			// Compute fractal Brownian motion
			for (int I = 0; I < Octaves; I++)
			{
				const float SampleX = (static_cast<float>(X) - static_cast<float>(Width) / 2 + Offset.x) / Scale *
					Frequency;
				const float SampleY = (static_cast<float>(Y) - static_cast<float>(Height) / 2 + Offset.y) / Scale *
					Frequency;

				const float NoiseValue = noise(SampleX, SampleY) * 2 - 1; // Range -1 to 1
				NoiseHeight += NoiseValue * Amplitude;

				Amplitude *= Persistence;
				Frequency *= Lacunarity;
			}

			// Track min and max for normalization
			if (NoiseHeight > MaxNoiseHeight) MaxNoiseHeight = NoiseHeight;
			if (NoiseHeight < MinNoiseHeight) MinNoiseHeight = NoiseHeight;

			NoiseMap[Y * Width + X] = NoiseHeight;
		}
	}

	// Normalize noise map to [0, 1]
	if (MaxNoiseHeight > MinNoiseHeight)
	{
		for (int I = 0; I < Width * Height; I++)
		{
			NoiseMap[I] = (NoiseMap[I] - MinNoiseHeight) / (MaxNoiseHeight - MinNoiseHeight);
		}
	}

	return NoiseMap;
}

bool PerlinNoise::saveAsRaw(const std::vector<float>& NoiseMap, const int Width, const int Height,
                            const std::string& Filename)
{
	try
	{
		std::ofstream File(Filename, std::ios::binary);
		if (!File.is_open())
		{
			std::cerr << "Failed to open file for writing: " << Filename << '\n';
			return false;
		}

		// Convert float values (0-1) to bytes (0-255)
		std::vector<unsigned char> ByteData(static_cast<size_t>(Width) * Height);
		for (int I = 0; I < Width * Height; I++)
		{
			ByteData[I] = static_cast<unsigned char>(NoiseMap[I] * 255.0f);
		}

		// Write raw bytes to file
		File.write(reinterpret_cast<const char*>(ByteData.data()), static_cast<long long>(ByteData.size()));
		File.close();

		std::cout << "Successfully saved RAW file: " << Filename << '\n';
		return true;
	}
	catch (const std::exception& E)
	{
		std::cerr << "Exception while saving RAW file: " << E.what() << '\n';
		return false;
	}
}

glm::vec3 PerlinNoise::applyColourGradient(const float NoiseValue, const std::vector<glm::vec3>& ColourGradient)
{
	// Ensure at least 2 colours in the gradient
	if (ColourGradient.size() < 2)
	{
		return glm::vec3(NoiseValue);
	}

	// Map noise value to gradient
	const float GradientPos = NoiseValue * static_cast<float>(ColourGradient.size() - 1);
	int Index = static_cast<int>(GradientPos);
	const float T = GradientPos - static_cast<float>(Index); // Fractional part for interpolation

	// Ensure index is within bounds
	Index = std::max(0, std::min(static_cast<int>(ColourGradient.size()) - 2, Index));

	// Interpolate between two colours
	const glm::vec3 Colour1 = ColourGradient[Index];
	const glm::vec3 Colour2 = ColourGradient[Index + 1];

	if (ColourGradient.empty())
	{
		return glm::vec3(NoiseValue);
	}
	if (ColourGradient.size() == 1)
	{
		return ColourGradient[0];
	}

	return Colour1 * (1 - T) + Colour2 * T;
}

bool PerlinNoise::saveAsJpg(const std::vector<float>& NoiseMap, const int Width, const int Height,
                            const std::string& Filename, const std::vector<glm::vec3>& ColourGradient)
{
	try
	{
		// Convert noise values to RGB using colour gradient
		std::vector<unsigned char> ImageData(static_cast<size_t>(Width) * Height * 3);

		for (int I = 0; I < Width * Height; I++)
		{
			const glm::vec3 Colour = applyColourGradient(NoiseMap[I], ColourGradient);

			// Convert floating point colour (0-1) to byte colour (0-255)
			ImageData[static_cast<unsigned long long>(I) * 3] = static_cast<unsigned char>(Colour.r * 255.0f); // R
			ImageData[I * 3 + 1] = static_cast<unsigned char>(Colour.g * 255.0f); // G
			ImageData[I * 3 + 2] = static_cast<unsigned char>(Colour.b * 255.0f); // B
		}

		// Save the image
		if (const int Result = stbi_write_jpg(Filename.c_str(), Width, Height, 3, ImageData.data(), 100); Result == 0)
		{
			std::cerr << "Failed to write JPG file: " << Filename << '\n';
			return false;
		}

		std::cout << "Successfully saved JPG file: " << Filename << '\n';
		return true;
	}
	catch (const std::exception& E)
	{
		std::cerr << "Exception while saving JPG file: " << E.what() << '\n';
		return false;
	}
}

GLuint PerlinNoise::createNoiseTexture(const std::vector<float>& NoiseMap, const int Width, const int Height,
                                       const std::vector<glm::vec3>& ColourGradient)
{
	GLuint TextureId;
	glGenTextures(1, &TextureId);
	glBindTexture(GL_TEXTURE_2D, TextureId);

	// Convert noise values to RGB using colour gradient
	std::vector<unsigned char> TextureData(static_cast<unsigned long long>(Width) * Height * 3);

	for (int I = 0; I < Width * Height; I++)
	{
		// Ensure Noise Map index is valid
		if (static_cast<unsigned long long>(I) < NoiseMap.size())
		{
			const glm::vec3 Colour = applyColourGradient(NoiseMap[I], ColourGradient);

			// Convert floating point colour (0-1) to byte colour (0-255)
			TextureData[static_cast<unsigned long long>(I) * 3] = static_cast<unsigned char>(Colour.r * 255.0f); // R
			TextureData[I * 3 + 1] = static_cast<unsigned char>(Colour.g * 255.0f); // G
			TextureData[I * 3 + 2] = static_cast<unsigned char>(Colour.b * 255.0f); // B
		}
		else
		{
			// Fill with red if out of bounds (for debugging)
			TextureData[static_cast<unsigned long long>(I) * 3] = 255; // R
			TextureData[I * 3 + 1] = 0; // G
			TextureData[I * 3 + 2] = 0; // B
		}
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureData.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	if (const GLenum Err = glGetError(); Err != GL_NO_ERROR)
	{
		std::cerr << "OpenGL error in createNoiseTexture: " << Err << '\n';
	}

	return TextureId;
}
