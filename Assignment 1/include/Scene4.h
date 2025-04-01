#pragma once
#include "Scene.h"
#include "Shader.h"
#include "Model.h"
#include "Skybox.h"
#include "Camera.h"
#include "LightManager.h"
#include "Terrain.h"
#include <iostream>

class Scene4 : public Scene {
public:
    Scene4(Camera& camera, LightManager& lightManager);
    void load() override;
    void update(float deltaTime) override;
    void render() override;
    void cleanup() override;

private:
    Shader LightingShader;
    Shader SkyboxShader;
    Shader TerrainShader;
    Model GardenPlant;
    Model Tree;
    Model Statue;
    Model Sphere;
    Skybox LSkybox;
    Camera& GCamera;
    LightManager& GLightManager;
    Material material;
    Terrain terrain;
};
