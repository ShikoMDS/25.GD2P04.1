/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2025 Media Design School

File Name : Scene3.h
Description : Declarations for perlin noise generation
Author : Ayoub Ahmad
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#pragma once
#include "Scene.h"
#include "Shader.h"
#include "Model.h"
#include "Skybox.h"
#include "Camera.h"
#include "PerlinNoise.h"
#include "Quad.h"
#include "Terrain.h"

#include <chrono>

class Scene3 final : public Scene
{
public:
	Scene3();
	void load() override;
	void update(float DeltaTime) override;
	void render() override;
	void cleanup() override;

private:
	Shader PvQuadShader;
	Shader PvAnimationShader;
	Skybox PvSkybox;

	PerlinNoise PvPerlinGenerator;
	std::vector<float> PvNoiseMap;
	std::vector<float> PvAnimatedNoiseMap;
	GLuint PvNoiseTexture = 0;
	GLuint PvAnimatedNoiseTexture = 0;
	Quad PvStaticNoiseQuad;
	Quad PvAnimatedNoiseQuad;
	Terrain PvNoiseTerrain;

	int PvNoiseWidth = 512;
	int PvNoiseHeight = 512;

	float PvAnimationTime = 0.0f;
	bool PvNoiseGenerated = false;

	std::vector<glm::vec3> PvFireColorGradient;

	void generatePerlinNoise();

	void updateAnimatedNoise(float DeltaTime);
};
