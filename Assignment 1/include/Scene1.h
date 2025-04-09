/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2025 Media Design School

File Name : Scene1.h
Description : Scene declarations for stencil test
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

constexpr float ModelScaleFactor = 0.01f;
constexpr float PlantScaleFactor = 0.005f;

class Scene1 final : public Scene
{
public:
	Scene1(Camera& Camera, LightManager& LightManager);

	void load() override;
	void update(float DeltaTime) override;
	void render() override;
	void cleanup() override;

private:
	Shader PvLightingShader;
	Shader PvSkyboxShader;
	Shader PvOutlineShader;

	Model PvGardenPlant;
	Model PvTree;
	Model PvStatue;
	Skybox PvSkybox;

	Camera* PvCamera;
	LightManager* PvLightManager;
	Material PvMaterial;

	float PvStatueRotation;
};
