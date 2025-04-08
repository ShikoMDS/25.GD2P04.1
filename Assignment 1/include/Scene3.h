#pragma once
#include "Scene.h"
#include "Shader.h"
#include "Model.h"
#include "Skybox.h"
#include "Camera.h"
#include "LightManager.h"
#include "PerlinNoise.h"
#include "Quad.h"
#include "Terrain.h"
#include <iostream>
#include <chrono>

class Scene3 : public Scene {
public:
    Scene3(Camera& camera, LightManager& lightManager);
    void load() override;
    void update(float deltaTime) override;
    void render() override;
    void cleanup() override;

private:
    // Standard scene components
    Shader QuadShader;         // Shader for rendering textured quads
    Shader AnimationShader;    // Shader for animated noise
    Skybox LSkybox;
    Camera& GCamera;
    LightManager& GLightManager;
    Material material;

    // Perlin noise specific components
    PerlinNoise perlinGenerator;
    std::vector<float> noiseMap;
    std::vector<float> animatedNoiseMap;
    GLuint noiseTexture = 0;
    GLuint animatedNoiseTexture = 0;
    Quad staticNoiseQuad;
    Quad animatedNoiseQuad;
    Terrain noiseTerrain;

    // Noise generation parameters
    int noiseWidth = 512;
    int noiseHeight = 512;
    float noiseScale = 50.0f;
    int noiseOctaves = 4;
    float noisePersistence = 0.5f;
    float noiseLacunarity = 2.0f;

    // Animation parameters
    float animationTime = 0.0f;
    bool noiseGenerated = false;

    // Color gradient for noise visualization (fire effect)
    std::vector<glm::vec3> fireColorGradient;

    // Generate Perlin noise and save to files
    void generatePerlinNoise();

    // Update animated noise
    void updateAnimatedNoise(float deltaTime);
};