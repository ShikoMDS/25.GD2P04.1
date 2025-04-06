#pragma once
#include "Scene.h"
#include "Shader.h"
#include "Model.h"
#include "Skybox.h"
#include "Camera.h"
#include "LightManager.h"
#include "Terrain.h"

class Scene2 : public Scene {
public:
    Scene2(Camera& camera, LightManager& lightManager);
    void load() override;
    void update(float deltaTime) override;
    void render() override;
    void cleanup() override;

private:
    GLuint loadTexture(const std::string& path);

    Shader LightingShader;
    Shader SkyboxShader;
    Shader TerrainShader;
    Model Sphere;
    Skybox LSkybox;
    Camera& GCamera;
    LightManager& GLightManager;
    Material material;
    Terrain terrain;

    // Terrain textures
    GLuint terrainTextures[4]; // Grass, Dirt, Rock, Snow
};