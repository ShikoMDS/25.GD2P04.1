#pragma once
#include "Scene.h"
#include "Shader.h"
#include "Model.h"
#include "Skybox.h"
#include "Camera.h"
#include "LightManager.h"

class Scene2 : public Scene {
public:
    Scene2(Camera& camera, LightManager& lightManager);
    void load() override;
    void update(float deltaTime) override;
    void render() override;
    void cleanup() override;

private:
    Shader LightingShader;
    Shader SkyboxShader;
    Model GardenPlant;
    Model Tree;
    Model Statue;
    Model Sphere;
    Skybox LSkybox;
    Camera& GCamera;
    LightManager& GLightManager;
    Material material;
};
