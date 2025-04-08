#include "SceneManager.h"
#include "Scene1.h"
#include "Scene2.h"
#include "Scene3.h"
#include "Scene4.h"
#include <iostream>

SceneManager::SceneManager(Camera& cam, LightManager& lightMgr)
    : camera(cam), lightManager(lightMgr), activeScene(SceneType::SCENE_1)
{
    // Load the initial scene (Scene1 by default)
    switchScene(SceneType::SCENE_1);
}

void SceneManager::resetCamera() {
    // Reset camera to default positions based on scene type
    switch (activeScene) {
    case SceneType::SCENE_1:
        // Good position for viewing the statue and trees
        camera.VPosition = glm::vec3(0.0f, 5.0f, 15.0f);
        camera.VFront = glm::vec3(0.0f, 0.0f, -1.0f);
        camera.VUp = glm::vec3(0.0f, 1.0f, 0.0f);
        camera.FYaw = -90.0f;
        camera.FPitch = -5.0f;
        break;
    case SceneType::SCENE_2:
        // Higher elevation to view the terrain
        camera.VPosition = glm::vec3(0.0f, 10.0f, 35.0f);
        camera.VFront = glm::vec3(0.0f, -0.5f, -1.0f);
        camera.VUp = glm::vec3(0.0f, 1.0f, 0.0f);
        camera.FYaw = -90.0f;
        camera.FPitch = -20.0f;
        break;
    case SceneType::SCENE_3:
        // Front view for Perlin noise visualization
        camera.VPosition = glm::vec3(0.0f, 0.0f, 3.0f);
        camera.VFront = glm::vec3(0.0f, 0.0f, -1.0f);
        camera.VUp = glm::vec3(0.0f, 1.0f, 0.0f);
        camera.FYaw = -90.0f;
        camera.FPitch = 0.0f;
        break;
    case SceneType::SCENE_4:
        // Position for viewing the post-processing scene
        camera.VPosition = glm::vec3(-1.0f, 5.0f, 32.5f);
        camera.VFront = glm::vec3(0.0f, 0.0f, -1.0f);
        camera.VUp = glm::vec3(0.0f, 1.0f, 0.0f);
        camera.FYaw = -90.0f;
        camera.FPitch = 0.0f;
        break;
    }

    // Force update of camera vectors
    camera.updateCameraVectors();
}

void SceneManager::switchScene(SceneType newScene) {
    // Only switch if the new scene is different from the current one
    if (!currentScene || activeScene != newScene) {
        // First cleanup the current scene if it exists
        if (currentScene) {
            std::cout << "Cleaning up current scene..." << std::endl;
            currentScene->cleanup();
            currentScene.reset(); // Properly reset the unique_ptr before creating a new one
        }

        std::cout << "Switching to scene type " << static_cast<int>(newScene) << std::endl;

        // Update the active scene type first (needed for proper camera reset)
        activeScene = newScene;

        // Reset camera for the new scene
        resetCamera();

        try {
            // Create the new scene based on the type
            switch (newScene) {
            case SceneType::SCENE_1:
                currentScene = std::make_unique<Scene1>(camera, lightManager);
                break;
            case SceneType::SCENE_2:
                currentScene = std::make_unique<Scene2>(camera, lightManager);
                break;
            case SceneType::SCENE_3:
                currentScene = std::make_unique<Scene3>(camera, lightManager);
                break;
            case SceneType::SCENE_4:
                currentScene = std::make_unique<Scene4>(camera, lightManager);
                break;
            default:
                std::cerr << "Invalid scene type!" << std::endl;
                return;
            }

            // Load the new scene resources
            currentScene->load();
        }
        catch (const std::exception& e) {
            std::cerr << "Error switching to scene: " << e.what() << std::endl;
            // Create a fallback scene if we had an error
            if (newScene != SceneType::SCENE_1) {
                std::cerr << "Falling back to Scene 1" << std::endl;
                currentScene = std::make_unique<Scene1>(camera, lightManager);
                currentScene->load();
                activeScene = SceneType::SCENE_1;
                resetCamera(); // Reset camera for fallback scene
            }
        }
    }
    else {
        std::cout << "Already in the active scene. No need to switch." << std::endl;
    }
}

void SceneManager::update(float deltaTime) {
    if (currentScene)
        currentScene->update(deltaTime);
}

void SceneManager::render() {
    if (currentScene)
        currentScene->render();
}

void SceneManager::cleanup() {
    if (currentScene) {
        currentScene->cleanup();
        currentScene.reset(); // Explicitly reset the unique_ptr
    }
}