/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2024 Media Design School

File Name : InputManager.h
Description : Definitions for window input in OpenGL
Author : Shikomisen (Ayoub Ahmad)
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#pragma once

#include "Camera.h"
#include "LightManager.h"

#include <glew.h>
#include <glfw3.h>
#include <unordered_map>

// Forward declaration of SceneManager
class SceneManager;

class InputManager
{
public:
    // Constructor now accepts a reference to SceneManager
    InputManager(Camera& Camera, LightManager& LightManager, SceneManager& sceneManager);

    void processInput(GLFWwindow* Window, float DeltaTime);
    void frameBufferSizeCallback(GLFWwindow* Window, int Width, int Height);
    void mouseCallback(GLFWwindow* Window, double PosX, double PosY);
    void scrollCallback(GLFWwindow* Window, double OffsetX, double OffsetY) const;
    void enableRawMouseMotion(GLFWwindow* window);
    void toggleWireframeMode();
    void toggleCursorVisibility(GLFWwindow* Window);

private:
    Camera& MCamera;
    LightManager& MLightManager;
    SceneManager& mSceneManager;  // Added reference to SceneManager

    bool MWireframe;
    bool MCursorVisible;
    bool MFirstMouse;

    double MLastX;
    double MLastY;

    std::unordered_map<int, bool> MKeyState;

    bool mAltDown = false;
};
