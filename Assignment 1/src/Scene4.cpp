/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2025 Media Design School

File Name : Scene4.cpp
Description : Implementations for frame buffers and post-processing
	requirements in Scene 4
Author : Ayoub Ahmad
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#include "Scene4.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <glfw3.h>
#include <iostream>

Scene4::Scene4(Camera& Camera, LightManager& LightManager)
	: PvLightingShader("resources/shaders/VertexShader.vert", "resources/shaders/FragmentShader.frag"),
	  PvSkyboxShader("resources/shaders/SkyboxVertexShader.vert", "resources/shaders/SkyboxFragmentShader.frag"),
	  PvTerrainShader("resources/shaders/TerrainVertexShader.vert", "resources/shaders/TerrainFragmentShader.frag"),
	  PvPostProcessingShader("resources/shaders/PostProcessingVertexShader.vert",
	                         "resources/shaders/PostProcessingFragmentShader.frag"),
	  PvGardenPlant("resources/models/AncientEmpire/SM_Env_Garden_Plants_01.obj",
	                "PolygonAncientWorlds_Texture_01_A.png"),
	  PvTree("resources/models/AncientEmpire/SM_Env_Tree_Palm_01.obj", "PolygonAncientWorlds_Texture_01_A.png"),
	  PvStatue("resources/models/AncientEmpire/SM_Prop_Statue_01.obj", "PolygonAncientWorlds_Texture_01_A.png"),
	  PvCamera(&Camera),
	  PvLightManager(&LightManager), PvMaterial(),
	  PvTerrain(HeightMapInfo{"resources/heightmap/Heightmap0.raw", 512, 512, 1.0f}),
	  PvStatueRotation(0.0f),
	  PvFramebuffer(0),
	  PvTextureColorBuffer(0),
	  PvRbo(0),
	  PvQuadVao(0),
	  PvQuadVbo(0),
	  PvCurrentEffect(0),
	  PvEffectTime(0.0f),
	  PvTabKeyPressed(false)
{
	PvTerrainTextures[0] = loadTexture("resources/textures/tileable_grass_00.png"); // Grass (lowest)
	PvTerrainTextures[1] = loadTexture("resources/textures/Dirt_04.png"); // Dirt/Soil
	PvTerrainTextures[2] = loadTexture("resources/textures/rck_2.png"); // Rock/Stone
	PvTerrainTextures[3] = loadTexture("resources/textures/snow01.png"); // Snow (highest)
}

void Scene4::load()
{
	std::cout << "Loading resources for Scene4..." << '\n';

	PvLightManager->initialize();

	PvMaterial.Ambient = glm::vec3(0.8f, 0.8f, 0.8f);
	PvMaterial.Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	PvMaterial.Specular = glm::vec3(0.5f, 0.5f, 0.5f);
	PvMaterial.Shininess = 32.0f;

	setupFramebuffer();
	setupScreenQuad();
}

GLuint Scene4::loadTexture(const std::string& Path)
{
	GLuint TextureId;
	glGenTextures(1, &TextureId);

	TextureId = textureFromFile(Path.c_str(), "resources/textures", false);

	return TextureId;
}

void Scene4::setupFramebuffer()
{
	if (PvFramebuffer != 0)
	{
		glDeleteFramebuffers(1, &PvFramebuffer);
		PvFramebuffer = 0;
	}

	if (PvTextureColorBuffer != 0)
	{
		glDeleteTextures(1, &PvTextureColorBuffer);
		PvTextureColorBuffer = 0;
	}

	if (PvRbo != 0)
	{
		glDeleteRenderbuffers(1, &PvRbo);
		PvRbo = 0;
	}

	glGenFramebuffers(1, &PvFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, PvFramebuffer);

	int Width = 800;
	int Height = 600;

	if (GLFWwindow* Window = glfwGetCurrentContext())
	{
		glfwGetFramebufferSize(Window, &Width, &Height);
	}

	// Create a texture to hold colour buffer
	glGenTextures(1, &PvTextureColorBuffer);
	glBindTexture(GL_TEXTURE_2D, PvTextureColorBuffer);

	// Use safe texture creation
	try
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Attach texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PvTextureColorBuffer, 0);

		// Create a renderbuffer object for depth and stencil attachments
		glGenRenderbuffers(1, &PvRbo);
		glBindRenderbuffer(GL_RENDERBUFFER, PvRbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Width, Height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		// Attach renderbuffer to framebuffer
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, PvRbo);

		// Check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "ERROR: Framebuffer is not complete!" << '\n';

			glDeleteFramebuffers(1, &PvFramebuffer);
			glDeleteTextures(1, &PvTextureColorBuffer);
			glDeleteRenderbuffers(1, &PvRbo);

			PvFramebuffer = 0;
			PvTextureColorBuffer = 0;
			PvRbo = 0;
		}
		else
		{
			std::cout << "Framebuffer set up successfully with dimensions: " << Width << "x" << Height << '\n';
		}
	}
	catch (...)
	{
		std::cerr << "Exception during framebuffer setup!" << '\n';

		glDeleteFramebuffers(1, &PvFramebuffer);
		glDeleteTextures(1, &PvTextureColorBuffer);
		glDeleteRenderbuffers(1, &PvRbo);

		PvFramebuffer = 0;
		PvTextureColorBuffer = 0;
		PvRbo = 0;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene4::setupScreenQuad()
{
	// Set up screen quad vertices (x, y, z, u, v)
	constexpr float QuadVertices[] = {
		// Positions        // Texture Coords
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,

		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f
	};

	// Set up VAO and VBO for screen quad
	glGenVertexArrays(1, &PvQuadVao);
	glGenBuffers(1, &PvQuadVbo);

	glBindVertexArray(PvQuadVao);
	glBindBuffer(GL_ARRAY_BUFFER, PvQuadVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertices), QuadVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), static_cast<void*>(nullptr));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));

	glBindVertexArray(0);

	//std::cout << "Screen quad set up successfully" << '\n';
}

void Scene4::cyclePostProcessingEffect()
{
	// Check for Tab key press in update function
	if (GLFWwindow* Window = glfwGetCurrentContext())
	{
		if (glfwGetKey(Window, GLFW_KEY_TAB) == GLFW_PRESS && !PvTabKeyPressed)
		{
			PvTabKeyPressed = true;
			PvCurrentEffect = (PvCurrentEffect + 1) % 5; // Cycle through 5 effects (including "no effect")

			// Print effect name for clarity
			std::string EffectName;
			switch (PvCurrentEffect)
			{
			case 0: EffectName = "Normal (no effect)";
				break;
			case 1: EffectName = "Color Inversion";
				break;
			case 2: EffectName = "Grayscale";
				break;
			case 3: EffectName = "Rain Effect";
				break;
			case 4: EffectName = "CRT Screen Effect";
				break;
			default: EffectName = "Unknown";
				break;
			}

			std::cout << "Switched to post-processing effect: " << EffectName << '\n';
		}
		else if (glfwGetKey(Window, GLFW_KEY_TAB) == GLFW_RELEASE)
		{
			PvTabKeyPressed = false;
		}
	}
}

void Scene4::update(const float DeltaTime)
{
	PvEffectTime += DeltaTime;

	PvStatueRotation += 45.0f * DeltaTime;
	if (PvStatueRotation > 360.0f)
		PvStatueRotation -= 360.0f;

	cyclePostProcessingEffect();
}

void Scene4::render()
{
	renderSceneToFramebuffer();

	renderPostProcessing();
}

void Scene4::renderSceneToFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, PvFramebuffer);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

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

	// ---------------------------
	// RENDER SKYBOX FIRST
	// ---------------------------
	PvSkybox.render(PvSkyboxShader, *PvCamera, Width, Height);

	// ---------------------------
	// RENDER TERRAIN 
	// ---------------------------
	PvTerrainShader.use();
	PvTerrainShader.setMat4("view", PvCamera->getViewMatrix());
	PvTerrainShader.setMat4("projection",
	                        PvCamera->getProjectionMatrix(static_cast<float>(Width), static_cast<float>(Height)));
	PvTerrainShader.setVec3("viewPos", PvCamera->PbPosition);

	PvTerrainShader.setVec3("directionalLight.direction", glm::vec3(0.3f, -0.9f, 0.3f));
	PvTerrainShader.setVec3("directionalLight.color", glm::vec3(1.0f, 0.95f, 0.8f));
	PvTerrainShader.setFloat("directionalLight.intensity", 2.5f);

	PvTerrainShader.setVec3("material.ambient", glm::vec3(0.7f, 0.7f, 0.7f));
	PvTerrainShader.setVec3("material.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
	PvTerrainShader.setVec3("material.specular", glm::vec3(0.1f, 0.1f, 0.1f));
	PvTerrainShader.setFloat("material.shininess", 8.0f);

	PvTerrainShader.setBool("useTextures", true);

	for (int I = 0; I < 4; I++)
	{
		glActiveTexture(GL_TEXTURE0 + I);
		glBindTexture(GL_TEXTURE_2D, PvTerrainTextures[I]);
		PvTerrainShader.setInt("terrainTextures[" + std::to_string(I) + "]", I);
	}

	PvTerrainShader.setVec3("terrainColors[0]", glm::vec3(0.1f, 0.7f, 0.1f)); // Brighter green for grass
	PvTerrainShader.setVec3("terrainColors[1]", glm::vec3(0.7f, 0.4f, 0.1f)); // Orange-brown for dirt
	PvTerrainShader.setVec3("terrainColors[2]", glm::vec3(0.8f, 0.8f, 0.7f)); // Light beige for rock
	PvTerrainShader.setVec3("terrainColors[3]", glm::vec3(1.0f, 1.0f, 1.0f)); // Pure white for snow

	PvTerrainShader.setFloat("heightLevels[0]", 0.0f); // Grass level (lowest)
	PvTerrainShader.setFloat("heightLevels[1]", 0.05f); // Dirt level
	PvTerrainShader.setFloat("heightLevels[2]", 0.15f); // Rock level
	PvTerrainShader.setFloat("heightLevels[3]", 0.225f); // Snow level (highest)
	PvTerrainShader.setFloat("blendFactor", 0.1f); // Moderate blending

	auto ModelMatrix = glm::mat4(1.0f);
	ModelMatrix = translate(ModelMatrix, glm::vec3(0.0f, 2.5f, 20.0f));
	ModelMatrix = scale(ModelMatrix, glm::vec3(0.025f, 0.004f, 0.025f));
	PvTerrainShader.setMat4("model", ModelMatrix);

	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	PvTerrain.drawTerrain();

	for (int I = 0; I < 4; I++)
	{
		glActiveTexture(GL_TEXTURE0 + I);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glActiveTexture(GL_TEXTURE0);

	// ---------------------------
	// RENDER OBJECTS 
	// ---------------------------
	PvLightingShader.use();
	PvLightingShader.setMat4("view", PvCamera->getViewMatrix());
	PvLightingShader.setMat4("projection",
	                         PvCamera->getProjectionMatrix(static_cast<float>(Width), static_cast<float>(Height)));
	PvLightingShader.setVec3("viewPos", PvCamera->PbPosition);
	PvLightingShader.setMaterial(PvMaterial);
	PvLightingShader.setBool("useTexture", true);
	PvLightManager->updateLighting(PvLightingShader);
	glActiveTexture(GL_TEXTURE0);

	constexpr glm::vec3 TreePositions[] = {
		{-3.0f, 2.55f, 22.0f}, // Tree left
		{1.0f, 2.55f, 22.0f}, // Tree right
		{-3.0f, 2.55f, 26.0f}, // Tree left front
		{1.0f, 2.55f, 26.0f} // Tree right front
	};

	for (auto TreePosition : TreePositions)
	{
		auto ModelMatrixTree = glm::mat4(1.0f);
		ModelMatrixTree = translate(ModelMatrixTree, TreePosition);
		ModelMatrixTree = scale(ModelMatrixTree, glm::vec3(0.004f));
		PvLightingShader.setMat4("model", ModelMatrixTree);
		PvTree.draw(PvLightingShader);
	}

	auto ModelMatrixStatue = glm::mat4(1.0f);
	ModelMatrixStatue = translate(ModelMatrixStatue, glm::vec3(-1.0f, 2.5f, 24.0f));
	ModelMatrixStatue = rotate(ModelMatrixStatue, glm::radians(PvStatueRotation), glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrixStatue = scale(ModelMatrixStatue, glm::vec3(0.004f));
	PvLightingShader.setMat4("model", ModelMatrixStatue);
	PvStatue.draw(PvLightingShader);

	for (int X = -4; X <= 4; X++)
	{
		for (int Z = 0; Z <= 9; Z++)
		{
			auto PlantMatrix = glm::mat4(1.0f);
			PlantMatrix = translate(PlantMatrix, glm::vec3(-1 + X * 0.35f, 2.5f, 22.5f + Z * 0.35f));
			PlantMatrix = scale(PlantMatrix, glm::vec3(0.002f));
			PvLightingShader.setMat4("model", PlantMatrix);
			PvGardenPlant.draw(PvLightingShader);
		}
	}
}

void Scene4::renderPostProcessing() const
{
	if (PvFramebuffer == 0 || PvTextureColorBuffer == 0)
	{
		std::cerr << "Cannot render post-processing: Invalid framebuffer resources" << '\n';
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	GLboolean DepthTestEnabled;
	GLboolean CullFaceEnabled;
	glGetBooleanv(GL_DEPTH_TEST, &DepthTestEnabled);
	glGetBooleanv(GL_CULL_FACE, &CullFaceEnabled);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	if (PvPostProcessingShader.getId() == 0)
	{
		std::cerr << "Cannot render post-processing: Invalid shader" << '\n';
		return;
	}

	PvPostProcessingShader.use();

	PvPostProcessingShader.setInt("screenTexture", 0);
	PvPostProcessingShader.setInt("effect", PvCurrentEffect);
	PvPostProcessingShader.setFloat("time", PvEffectTime);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, PvTextureColorBuffer);

	if (PvQuadVao == 0)
	{
		std::cerr << "Cannot render post-processing: Invalid quad VAO" << '\n';
		return;
	}

	glBindVertexArray(PvQuadVao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);

	if (DepthTestEnabled) glEnable(GL_DEPTH_TEST);
	if (CullFaceEnabled) glEnable(GL_CULL_FACE);
}

void Scene4::cleanup()
{
	std::cout << "Cleaning up Scene4 resources..." << '\n';

	if (PvLightingShader.getId() != 0)
	{
		glDeleteProgram(PvLightingShader.getId());
		PvLightingShader.PbId = 0;
	}
	if (PvSkyboxShader.getId() != 0)
	{
		glDeleteProgram(PvSkyboxShader.getId());
		PvSkyboxShader.PbId = 0;
	}
	if (PvTerrainShader.getId() != 0)
	{
		glDeleteProgram(PvTerrainShader.getId());
		PvTerrainShader.PbId = 0;
	}
	if (PvPostProcessingShader.getId() != 0)
	{
		glDeleteProgram(PvPostProcessingShader.getId());
		PvPostProcessingShader.PbId = 0;
	}

	PvGardenPlant.cleanup();
	PvTree.cleanup();
	PvStatue.cleanup();
	PvSkybox.cleanup();

	for (unsigned int& PvTerrainTexture : PvTerrainTextures)
	{
		if (glIsTexture(PvTerrainTexture))
		{
			glDeleteTextures(1, &PvTerrainTexture);
			PvTerrainTexture = 0;
		}
	}

	if (glIsFramebuffer(PvFramebuffer))
	{
		glDeleteFramebuffers(1, &PvFramebuffer);
		PvFramebuffer = 0;
	}

	if (glIsTexture(PvTextureColorBuffer))
	{
		glDeleteTextures(1, &PvTextureColorBuffer);
		PvTextureColorBuffer = 0;
	}

	if (glIsRenderbuffer(PvRbo))
	{
		glDeleteRenderbuffers(1, &PvRbo);
		PvRbo = 0;
	}

	if (glIsVertexArray(PvQuadVao))
	{
		glDeleteVertexArrays(1, &PvQuadVao);
		PvQuadVao = 0;
	}

	if (glIsBuffer(PvQuadVbo))
	{
		glDeleteBuffers(1, &PvQuadVbo);
		PvQuadVbo = 0;
	}

	std::cout << "Scene4 cleanup complete" << '\n';
}
