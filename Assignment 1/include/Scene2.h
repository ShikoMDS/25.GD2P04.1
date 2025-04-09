/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2025 Media Design School

File Name : Scene2.h
Description : Declarations for terrain rendering scene
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

class Scene2 final : public Scene {
public:
    Scene2(Camera& Camera, LightManager& LightManager);
    void load() override;
    void update(float DeltaTime) override;
    void render() override;
    void cleanup() override;

private:
    static GLuint loadTexture(const std::string& Path);

    Shader PvLightingShader;
    Shader PvSkyboxShader;
    Shader PvTerrainShader;
    Skybox PvSkybox;
    Camera* PvCamera;
    LightManager* PvLightManager;
    Material PvMaterial;
    Terrain PvTerrain;

    GLuint PvTerrainTextures[4];
};