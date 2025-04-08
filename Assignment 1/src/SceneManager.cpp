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

void SceneManager::switchScene(SceneType newScene) {
    if (!currentScene || activeScene != newScene) {
        if (currentScene) {
            std::cout << "Cleaning up current scene..." << std::endl;
            currentScene->cleanup();
        }

        camera.reset();

        std::cout << "Switching to scene type " << static_cast<int>(newScene) << std::endl;
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
        currentScene->load();
        activeScene = newScene;
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
    if (currentScene)
        currentScene->cleanup();
}
