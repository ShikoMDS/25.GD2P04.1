#pragma once
#include "Scene.h"
#include "Shader.h"
#include "Model.h"
#include "Skybox.h"
#include "Camera.h"
#include "LightManager.h"
#include <iostream>

// Scale factors (using the previously working values)
constexpr float ModelScaleFactor = 0.01f;
constexpr float PlantScaleFactor = 0.005f;

class Scene1 : public Scene {
public:
    Scene1(Camera& camera, LightManager& lightManager);

    void load() override;
    void update(float deltaTime) override;
    void render() override;
    void cleanup() override;

private:
    Shader LightingShader;
    Shader SkyboxShader;
    Shader OutlineShader;  // For the stencil outline effect

    Model GardenPlant;
    Model Tree;
    Model Statue;
    Skybox LSkybox;

    Camera& GCamera;
    LightManager& GLightManager;
    Material material;

    float mStatueRotation; // Rotation angle for the statue (in degrees)
};
