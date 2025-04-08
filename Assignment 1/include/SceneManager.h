#pragma once
#include <memory>
#include "Scene.h"
#include "Camera.h"
#include "LightManager.h"

class SceneManager {
public:
    SceneManager(Camera& camera, LightManager& lightManager);

    // Switch to a new scene (SCENE_1, SCENE_2, SCENE_3, SCENE_4)
    void switchScene(SceneType newScene);

    // Reset camera position based on current scene
    void resetCamera();

    // Update and render the current scene
    void update(float deltaTime);
    void render();

    // Cleanup current scene resources
    void cleanup();

private:
    std::unique_ptr<Scene> currentScene;
    SceneType activeScene;
    Camera& camera;
    LightManager& lightManager;
};