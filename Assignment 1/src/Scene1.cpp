/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2025 Media Design School

File Name : Scene1.cpp
Description : Implementations for stencil test requirements in Scene 1
Author : Ayoub Ahmad
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#include "Scene1.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>

void checkGlError(const char* Label)
{
	GLenum Err;
	while ((Err = glGetError()) != GL_NO_ERROR)
	{
		std::cerr << "OpenGL error (" << Label << "): " << Err << '\n';
	}
}

Scene1::Scene1(Camera& Camera, LightManager& LightManager)
	: PvLightingShader("resources/shaders/VertexShader.vert", "resources/shaders/FragmentShader.frag"),
	  PvSkyboxShader("resources/shaders/SkyboxVertexShader.vert", "resources/shaders/SkyboxFragmentShader.frag"),
	  PvOutlineShader("resources/shaders/OutlineVertexShader.vert", "resources/shaders/OutlineFragmentShader.frag"),
	  PvGardenPlant("resources/models/AncientEmpire/SM_Env_Garden_Plants_01.obj",
	                "PolygonAncientWorlds_Texture_01_A.png"),
	  PvTree("resources/models/AncientEmpire/SM_Env_Tree_Palm_01.obj", "PolygonAncientWorlds_Texture_01_A.png"),
	  PvStatue("resources/models/AncientEmpire/SM_Prop_Statue_01.obj", "PolygonAncientWorlds_Texture_01_A.png"),
	  PvCamera(&Camera),
	  PvLightManager(&LightManager), PvMaterial(),
	  PvStatueRotation(0.0f)
{
}

void Scene1::load()
{
	std::cout << "Loading resources for Scene1..." << '\n';
	PvLightManager->initialize();

	PvMaterial.Ambient = glm::vec3(1.0f, 1.0f, 1.0f);
	PvMaterial.Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	PvMaterial.Specular = glm::vec3(0.5f, 0.5f, 0.5f);
	PvMaterial.Shininess = 32.0f;
}

void Scene1::update(const float DeltaTime)
{
	PvStatueRotation += 45.0f * DeltaTime;
	if (PvStatueRotation > 360.0f)
		PvStatueRotation -= 360.0f;
}

void Scene1::render()
{
	// 1) Clear color, depth, and stencil buffers
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	// ----------------------------------------------------------------
	// (A) Colored Pass: Render the full scene normally
	// ----------------------------------------------------------------
	PvLightingShader.use();
	PvLightingShader.setMat4("view", PvCamera->getViewMatrix());
	PvLightingShader.setMat4("projection", PvCamera->getProjectionMatrix(800, 600));
	PvLightingShader.setVec3("viewPos", PvCamera->PbPosition);
	PvLightingShader.setMaterial(PvMaterial);
	PvLightingShader.setBool("useTexture", true);
	PvLightManager->updateLighting(PvLightingShader);
	glActiveTexture(GL_TEXTURE0);

	constexpr glm::vec3 TreePositions[] = {
		{-1.5f, 0.0f, -1.5f},
		{-1.5f, 0.0f, 1.5f},
		{1.5f, 0.0f, -1.5f},
		{1.5f, 0.0f, 1.5f}
	};

	for (int X = -3; X <= 3; X++)
	{
		for (int Z = -3; Z <= 3; Z++)
		{
			auto ModelMatrix = glm::mat4(1.0f);
			ModelMatrix = translate(ModelMatrix, glm::vec3(X * 0.8f, -0.2f, Z * 0.8f));
			ModelMatrix = scale(ModelMatrix, glm::vec3(0.004f));
			PvLightingShader.setMat4("model", ModelMatrix);
			PvGardenPlant.draw(PvLightingShader);
		}
	}

	glm::mat4 ModelMatrixTrees[4];

	auto ModelMatrixStatue = glm::mat4(1.0f);
	ModelMatrixStatue = translate(ModelMatrixStatue, glm::vec3(0.0f, 0.0f, 0.0f));
	ModelMatrixStatue = rotate(ModelMatrixStatue, glm::radians(PvStatueRotation), glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrixStatue = scale(ModelMatrixStatue, glm::vec3(0.015f));
	PvLightingShader.setMat4("model", ModelMatrixStatue);
	PvStatue.draw(PvLightingShader);

	for (int I = 0; I < 4; I++)
	{
		ModelMatrixTrees[I] = glm::mat4(1.0f);
		ModelMatrixTrees[I] = translate(ModelMatrixTrees[I], TreePositions[I]);
		ModelMatrixTrees[I] = scale(ModelMatrixTrees[I], glm::vec3(0.01f));
		PvLightingShader.setMat4("model", ModelMatrixTrees[I]);
		PvTree.draw(PvLightingShader);
	}

	PvSkybox.render(PvSkyboxShader, *PvCamera, 800, 600);

	// ----------------------------------------------------------------
	// (B) Stencil Update Pass: Mark all pixels of outlined objects
	// ----------------------------------------------------------------
	glEnable(GL_STENCIL_TEST);
	// Clear stencil buffer
	glClear(GL_STENCIL_BUFFER_BIT);
	glStencilMask(0xFF); // enable writing to stencil buffer

	// Disable color and depth writes and disable depth test for stencil pass
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	// Set stencil operation: always replace with 1
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);

	// Draw outlined objects into stencil buffer
	PvLightingShader.use();

	// Draw statue into stencil buffer
	PvLightingShader.setMat4("model", ModelMatrixStatue);
	PvStatue.draw(PvLightingShader);

	// Draw trees into stencil buffer
	for (const auto& ModelMatrixTree : ModelMatrixTrees)
	{
		PvLightingShader.setMat4("model", ModelMatrixTree);
		PvTree.draw(PvLightingShader);
	}

	// Restore color and depth writes, and re-enable depth test
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	// ----------------------------------------------------------------
	// (C) Outline Pass: Render blue outline where stencil != 1
	// ----------------------------------------------------------------
	// Only draw where stencil value is NOT equal to 1 (i.e. around the borders)
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00); // Disable writing to stencil
	glDepthFunc(GL_ALWAYS); // Force outline to draw on top

	PvOutlineShader.use();
	PvOutlineShader.setMat4("view", PvCamera->getViewMatrix());
	PvOutlineShader.setMat4("projection", PvCamera->getProjectionMatrix(800, 600));
	PvOutlineShader.setVec3("outlineColor", glm::vec3(0.0f, 0.0f, 1.0f));

	const glm::mat4 OutlineMatrixStatue = scale(ModelMatrixStatue, glm::vec3(1.03f));
	PvOutlineShader.setMat4("model", OutlineMatrixStatue);
	PvStatue.draw(PvOutlineShader);

	for (const auto& ModelMatrixTree : ModelMatrixTrees)
	{
		glm::mat4 OutlineMatrixTree = scale(ModelMatrixTree, glm::vec3(1.03f));
		PvOutlineShader.setMat4("model", OutlineMatrixTree);
		PvTree.draw(PvOutlineShader);
	}

	// ----------------------------------------------------------------
	// (D) Reset State
	// ----------------------------------------------------------------
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glStencilMask(0xFF);
	glDisable(GL_STENCIL_TEST);
}

void Scene1::cleanup()
{
	std::cout << "Cleaning up Scene1 resources..." << '\n';
	if (PvLightingShader.getId() != 0)
		glDeleteProgram(PvLightingShader.getId());
	if (PvSkyboxShader.getId() != 0)
		glDeleteProgram(PvSkyboxShader.getId());
	if (PvOutlineShader.getId() != 0)
		glDeleteProgram(PvOutlineShader.getId());

	PvGardenPlant.cleanup();
	PvTree.cleanup();
	PvStatue.cleanup();
	PvSkybox.cleanup();
}
