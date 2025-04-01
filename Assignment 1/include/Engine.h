#pragma once
#include "Camera.h"
#include "LightManager.h"
#include "InputManager.h"
#include "SceneManager.h"
#include <glfw3.h>

class Engine {
public:
    Engine(GLFWwindow* window);
    void run();
private:
    GLFWwindow* window;
    Camera camera;
    LightManager lightManager;
    SceneManager sceneManager;
    InputManager inputManager;
};
