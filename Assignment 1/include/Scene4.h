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
    // Helper functions
    void setupFramebuffer();
    void setupScreenQuad();
    void cyclePostProcessingEffect();
    void renderSceneToFramebuffer();
    void renderPostProcessing();

    // Models and rendering (reused from Scenes 1 and 2)
    Shader LightingShader;      // Scene 1 - object shader
    Shader SkyboxShader;        // Both scenes
    Shader TerrainShader;       // Scene 2 - terrain shader
    Shader PostProcessingShader;// Scene 4 - post-processing

    Model GardenPlant;
    Model Tree;
    Model Statue;
    Skybox LSkybox;

    Camera& GCamera;
    LightManager& GLightManager;
    Material material;
    Terrain terrain;

    // Rotation angle for statue (from Scene 1)
    float mStatueRotation;

    // Framebuffer objects
    GLuint framebuffer;
    GLuint textureColorBuffer;
    GLuint rbo;

    // Screen quad for post-processing
    GLuint quadVAO, quadVBO;

    // Post-processing state
    int currentEffect;    // 0 = normal, 1 = inversion, 2 = grayscale, 3 = rain, 4 = custom
    float effectTime;     // For time-based effects
    bool tabKeyPressed;   // To detect key press and release

    // Terrain textures (from Scene 2)
    GLuint terrainTextures[4]; // Grass, Dirt, Rock, Snow
    GLuint loadTexture(const std::string& path);
};