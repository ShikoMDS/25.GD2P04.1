/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2025 Media Design School

File Name : Scene3.cpp
Description : Implementations for perlin noise generation requirements
	in Scene 3
Author : Ayoub Ahmad
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#include "Scene3.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <ctime>
#include <glew.h>
#include <glfw3.h>

// Helper function to ensure directory exists
void ensureDirectoryExists(const std::string& Path)
{
	// Extract directory path
	const size_t Pos = Path.find_last_of("/\\");
	if (Pos == std::string::npos)
	{
		return; // No directory component
	}

	const std::string Dir = Path.substr(0, Pos);

	// Create directory if it doesn't exist
#ifdef _WIN32
	// Windows
	const std::string Cmd = "if not exist \"" + Dir + "\" mkdir \"" + Dir + "\"";
	system(Cmd.c_str());
#else
// Linux/Unix
    std::string cmd = "mkdir -p \"" + dir + "\"";
    system(cmd.c_str());
#endif

	std::cout << "Ensuring directory exists: " << Dir << '\n';
}

Scene3::Scene3()
	: PvQuadShader("resources/shaders/QuadVertexShader.vert", "resources/shaders/QuadFragmentShader.frag"),
	  PvAnimationShader("resources/shaders/AnimationVertexShader.vert",
	                    "resources/shaders/AnimationFragmentShader.frag"),
	  PvPerlinGenerator(static_cast<unsigned int>(std::time(nullptr))),
	  PvNoiseTerrain(HeightMapInfo{"resources/heightmap/Heightmap0.raw", 512, 512, 1.0f})
{
	//std::cout << "Scene3 constructor called" << '\n';

	// Define fire color gradient for noise visualization
	PvFireColorGradient = {
		glm::vec3(0.0f, 0.0f, 0.0f), // Black (low values)
		glm::vec3(0.5f, 0.0f, 0.0f), // Dark red
		glm::vec3(1.0f, 0.0f, 0.0f), // Red
		glm::vec3(1.0f, 0.5f, 0.0f), // Orange
		glm::vec3(1.0f, 1.0f, 0.0f), // Yellow
		glm::vec3(1.0f, 1.0f, 1.0f) // White (high values)
	};

	PvNoiseTexture = 0;
	PvAnimatedNoiseTexture = 0;
}

void Scene3::load()
{
	//std::cout << "Loading resources for Scene3..." << '\n';

	generatePerlinNoise();
	PvNoiseGenerated = true;

	//std::cout << "Perlin noise generated and saved successfully." << '\n';
}

void Scene3::generatePerlinNoise()
{
	//std::cout << "Generating Perlin noise..." << '\n';

	try
	{
		// Generate the noise map with good distribution
		PvNoiseMap = PvPerlinGenerator.generateNoiseMap(
			PvNoiseWidth,
			PvNoiseHeight,
			50.0f, // Larger scale for broader features
			3, // Fewer octaves for less detail
			0.5f, // Persistence
			2.0f // Lacunarity
		);

		// Ensure directories exist before saving files
		const std::string RawFilePath = "resources/heightmap/perlin_noise.raw";
		const std::string JpgFilePath = "resources/heightmap/perlin_noise.jpg";

		ensureDirectoryExists(RawFilePath);
		ensureDirectoryExists(JpgFilePath);

		// Save noise map as a RAW file for terrain heightmap
		std::cout << "Saving RAW heightmap to: " << RawFilePath << '\n';
		PerlinNoise::saveAsRaw(PvNoiseMap, PvNoiseWidth, PvNoiseHeight, RawFilePath);

		// Save noise map as a JPG file for visualization
		std::cout << "Saving JPG visualization to: " << JpgFilePath << '\n';
		PerlinNoise::saveAsJpg(PvNoiseMap, PvNoiseWidth, PvNoiseHeight, JpgFilePath, PvFireColorGradient);

		// Delete any existing textures before creating new ones
		if (PvNoiseTexture != 0)
		{
			glDeleteTextures(1, &PvNoiseTexture);
			PvNoiseTexture = 0;
		}

		// Create OpenGL texture for static noise quad
		PvNoiseTexture = PerlinNoise::createNoiseTexture(PvNoiseMap, PvNoiseWidth, PvNoiseHeight, PvFireColorGradient);
		if (PvNoiseTexture != 0)
		{
			std::cout << "Created static noise texture with ID: " << PvNoiseTexture << '\n';
		}
		else
		{
			std::cerr << "FAILED to create static noise texture!" << '\n';
		}

		// Initialize animated noise with same parameters
		PvAnimatedNoiseMap = PvNoiseMap; // Start with same noise

		// Delete any existing animated texture before creating a new one
		if (PvAnimatedNoiseTexture != 0)
		{
			glDeleteTextures(1, &PvAnimatedNoiseTexture);
			PvAnimatedNoiseTexture = 0;
		}

		PvAnimatedNoiseTexture = PerlinNoise::createNoiseTexture(PvAnimatedNoiseMap, PvNoiseWidth, PvNoiseHeight,
		                                                         PvFireColorGradient);
		if (PvAnimatedNoiseTexture != 0)
		{
			std::cout << "Created animated noise texture with ID: " << PvAnimatedNoiseTexture << '\n';
		}
		else
		{
			std::cerr << "FAILED to create animated noise texture!" << '\n';
		}

		//std::cout << "Perlin noise generated and saved successfully." << '\n';
	}
	catch (const std::exception& E)
	{
		std::cerr << "Error generating Perlin noise: " << E.what() << '\n';
	}
}

void Scene3::update(const float DeltaTime)
{
	// Update animation time
	PvAnimationTime += DeltaTime;

	// Update animated texture every 0.1 seconds
	static float TimeSinceLastUpdate = 0.0f;
	TimeSinceLastUpdate += DeltaTime;

	if (TimeSinceLastUpdate > 0.1f)
	{
		updateAnimatedNoise(TimeSinceLastUpdate);
		TimeSinceLastUpdate = 0.0f;
	}
}

void Scene3::updateAnimatedNoise(float DeltaTime)
{
	// Only update if we have valid noise data
	if (PvAnimatedNoiseMap.empty())
	{
		return;
	}

	// Create time-dependent offset for noise
	const glm::vec2 Offset(
		PvAnimationTime * 0.5f, // Steady horizontal movement
		sin(PvAnimationTime * 0.3f) * 3.0f // Oscillating vertical movement
	);

	try
	{
		// Generate new noise with time-based offset and parameters
		PvAnimatedNoiseMap = PvPerlinGenerator.generateNoiseMap(
			PvNoiseWidth,
			PvNoiseHeight,
			50.0f + sin(PvAnimationTime * 0.2f) * 10.0f, // Varying scale
			3,
			0.5f,
			2.0f,
			Offset
		);

		// Delete old texture first if it exists
		if (glIsTexture(PvAnimatedNoiseTexture))
		{
			glDeleteTextures(1, &PvAnimatedNoiseTexture);
		}

		// Create new texture - use a more dramatic fire gradient for animation
		const std::vector AnimatedGradient = {
			glm::vec3(0.0f, 0.0f, 0.0f), // Black (low values)
			glm::vec3(0.5f, 0.0f, 0.0f), // Dark red
			glm::vec3(0.7f, 0.0f, 0.0f), // Red
			glm::vec3(1.0f, 0.3f, 0.0f), // Dark orange
			glm::vec3(1.0f, 0.5f, 0.0f), // Orange
			glm::vec3(1.0f, 0.7f, 0.0f), // Light orange
			glm::vec3(1.0f, 1.0f, 0.3f), // Yellow
			glm::vec3(1.0f, 1.0f, 0.7f), // Light yellow
			glm::vec3(1.0f, 1.0f, 1.0f) // White (high values)
		};

		PvAnimatedNoiseTexture = PerlinNoise::createNoiseTexture(
			PvAnimatedNoiseMap, PvNoiseWidth, PvNoiseHeight, AnimatedGradient
		);
	}
	catch (const std::exception& E)
	{
		std::cerr << "Error updating animated noise: " << E.what() << '\n';
	}
}

void Scene3::render()
{
	glClearColor(0.0f, 0.05f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	while (glGetError() != GL_NO_ERROR)
	{
	}

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	// Get window size for proper aspect ratio
	int Width, Height;
	if (GLFWwindow* Window = glfwGetCurrentContext())
	{
		glfwGetFramebufferSize(Window, &Width, &Height);
	}
	else
	{
		Width = 800;
		Height = 600;
	}
	const float AspectRatio = static_cast<float>(Width) / static_cast<float>(Height);

	glm::mat4 Projection;
	if (AspectRatio >= 1.0f)
	{
		Projection = glm::ortho(-AspectRatio, AspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
	}
	else
	{
		Projection = glm::ortho(-1.0f, 1.0f, -1.0f / AspectRatio, 1.0f / AspectRatio, -1.0f, 1.0f);
	}

	constexpr auto View = glm::mat4(1.0f);
	constexpr auto Model = glm::mat4(1.0f);

	constexpr float LeftOffset = 0.5f;
	constexpr float QuadSize = 0.5f; // Size of each quad (in normalized coordinates)
	constexpr float Gap = 1.2f; // Gap between quads
	constexpr float LeftQuadX = -QuadSize - Gap / 2 + LeftOffset; // Position of left quad
	constexpr float RightQuadX = Gap / 2; // Position of right quad

	// Quad 1
	if (PvNoiseTexture > 0 && glIsTexture(PvNoiseTexture))
	{
		if (PvQuadShader.getId() != 0)
		{
			PvQuadShader.use();

			// Position quad on left side with proper gap
			glm::mat4 LeftQuadModel = translate(Model, glm::vec3(LeftQuadX, 0.0f, 0.0f));
			LeftQuadModel = scale(LeftQuadModel, glm::vec3(QuadSize, QuadSize, 1.0f));

			PvQuadShader.setMat4("model", LeftQuadModel);
			PvQuadShader.setMat4("view", View);
			PvQuadShader.setMat4("projection", Projection);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, PvNoiseTexture);
			PvQuadShader.setInt("texture1", 0);

			PvStaticNoiseQuad.draw(PvQuadShader, PvNoiseTexture);
		}
	}

	// Quad 2
	if (PvAnimatedNoiseTexture > 0 && glIsTexture(PvAnimatedNoiseTexture))
	{
		if (PvAnimationShader.getId() != 0)
		{
			PvAnimationShader.use();

			// Position quad on right side with proper gap
			glm::mat4 RightQuadModel = translate(Model, glm::vec3(RightQuadX, 0.0f, 0.0f));
			RightQuadModel = scale(RightQuadModel, glm::vec3(QuadSize, QuadSize, 1.0f));

			PvAnimationShader.setMat4("model", RightQuadModel);
			PvAnimationShader.setMat4("view", View);
			PvAnimationShader.setMat4("projection", Projection);
			PvAnimationShader.setFloat("time", PvAnimationTime);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, PvAnimatedNoiseTexture);
			PvAnimationShader.setInt("texture1", 0);

			PvAnimatedNoiseQuad.draw(PvAnimationShader, PvAnimatedNoiseTexture);
		}
	}

	glEnable(GL_DEPTH_TEST);
}

void Scene3::cleanup()
{
	//std::cout << "Cleaning up Scene3 resources..." << '\n';

	if (PvQuadShader.getId() != 0)
	{
		glDeleteProgram(PvQuadShader.getId());
	}

	if (PvAnimationShader.getId() != 0)
	{
		glDeleteProgram(PvAnimationShader.getId());
	}

	if (glIsTexture(PvNoiseTexture))
	{
		glDeleteTextures(1, &PvNoiseTexture);
		PvNoiseTexture = 0;
	}

	if (glIsTexture(PvAnimatedNoiseTexture))
	{
		glDeleteTextures(1, &PvAnimatedNoiseTexture);
		PvAnimatedNoiseTexture = 0;
	}

	PvStaticNoiseQuad.cleanup();
	PvAnimatedNoiseQuad.cleanup();

	PvNoiseMap.clear();
	PvAnimatedNoiseMap.clear();

	PvSkybox.cleanup();

	//std::cout << "Scene3 cleanup completed" << '\n';
}
