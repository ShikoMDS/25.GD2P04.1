/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2025 Media Design School

File Name : Scene4.h
Description : Declarations for frame buffers and post-processing
Author : Ayoub Ahmad
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#pragma once
#include "Scene.h"
#include "Shader.h"
#include "Model.h"
#include "Skybox.h"
#include "Camera.h"
#include "LightManager.h"
#include "Terrain.h"
#include <iostream>

class Scene4 final : public Scene
{
public:
	Scene4(Camera& Camera, LightManager& LightManager);
	void load() override;
	void update(float DeltaTime) override;
	void render() override;
	void cleanup() override;

private:
	void setupFramebuffer();
	void setupScreenQuad();
	void cyclePostProcessingEffect();
	void renderSceneToFramebuffer();
	void renderPostProcessing() const;

	Shader PvLightingShader;
	Shader PvSkyboxShader;
	Shader PvTerrainShader;
	Shader PvPostProcessingShader;

	Model PvGardenPlant;
	Model PvTree;
	Model PvStatue;
	Skybox PvSkybox;

	Camera* PvCamera;
	LightManager* PvLightManager;
	Material PvMaterial;
	Terrain PvTerrain;

	float PvStatueRotation;

	GLuint PvFramebuffer;
	GLuint PvTextureColorBuffer;
	GLuint PvRbo;

	GLuint PvQuadVao, PvQuadVbo;

	int PvCurrentEffect;
	float PvEffectTime;
	bool PvTabKeyPressed;

	GLuint PvTerrainTextures[4];
	static GLuint loadTexture(const std::string& Path);
};
