/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2025 Media Design School

File Name : Scene2.cpp
Description : Implementations for terrain rendering requirements in Scene 2
Author : Ayoub Ahmad
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#include "Scene2.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <iostream>

Scene2::Scene2(Camera& Camera, LightManager& LightManager)
	: PvLightingShader("resources/shaders/VertexShader.vert", "resources/shaders/FragmentShader.frag"),
	  PvSkyboxShader("resources/shaders/SkyboxVertexShader.vert", "resources/shaders/SkyboxFragmentShader.frag"),
	  PvTerrainShader("resources/shaders/TerrainVertexShader.vert", "resources/shaders/TerrainFragmentShader.frag"),
	  PvCamera(&Camera),
	  PvLightManager(&LightManager), PvMaterial(),
	  PvTerrain(HeightMapInfo{"resources/heightmap/Heightmap0.raw", 512, 512, 1.0f})
{
	PvTerrainTextures[0] = loadTexture("resources/textures/tileable_grass_00.png"); // Grass (lowest)
	PvTerrainTextures[1] = loadTexture("resources/textures/Dirt_04.png"); // Dirt/Soil
	PvTerrainTextures[2] = loadTexture("resources/textures/rck_2.png"); // Rock/Stone
	PvTerrainTextures[3] = loadTexture("resources/textures/snow01.png"); // Snow (highest)
}

void Scene2::load()
{
	std::cout << "Loading resources for Scene2..." << '\n';

	PvLightManager->initialize();

	PvMaterial.Ambient = glm::vec3(0.75f, 0.75f, 0.75f);
	PvMaterial.Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	PvMaterial.Specular = glm::vec3(0.2f, 0.2f, 0.2f);
	PvMaterial.Shininess = 16.0f;
}

void Scene2::update(float DeltaTime)
{
}

void Scene2::render()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	PvSkybox.render(PvSkyboxShader, *PvCamera, 800, 600);

	PvTerrainShader.use();
	PvTerrainShader.setMat4("view", PvCamera->getViewMatrix());
	PvTerrainShader.setMat4("projection", PvCamera->getProjectionMatrix(800, 600));
	PvTerrainShader.setVec3("viewPos", PvCamera->PbPosition);

	PvTerrainShader.setVec3("directionalLight.direction", glm::vec3(0.4f, -0.8f, 0.4f));
	PvTerrainShader.setVec3("directionalLight.color", glm::vec3(1.0f, 1.0f, 1.0f));
	PvTerrainShader.setFloat("directionalLight.intensity", 2.0f);

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
}

void Scene2::cleanup()
{
	std::cout << "Cleaning up Scene2 resources..." << '\n';

	if (PvLightingShader.getId() != 0)
		glDeleteProgram(PvLightingShader.getId());
	if (PvSkyboxShader.getId() != 0)
		glDeleteProgram(PvSkyboxShader.getId());
	if (PvTerrainShader.getId() != 0)
		glDeleteProgram(PvTerrainShader.getId());

	for (unsigned int& PvTerrainTexture : PvTerrainTextures)
	{
		if (PvTerrainTexture != 0)
			glDeleteTextures(1, &PvTerrainTexture);
	}

	PvSkybox.cleanup();
}

GLuint Scene2::loadTexture(const std::string& Path)
{
	GLuint TextureId;
	glGenTextures(1, &TextureId);

	TextureId = textureFromFile(Path.c_str(), "resources/textures", false);

	return TextureId;
}
