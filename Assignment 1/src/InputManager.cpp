#include "InputManager.h"
#include "SceneManager.h"  // Include the header for SceneManager
#include <iostream>
#include <glfw3.h>

InputManager::InputManager(Camera& camera, LightManager& lightManager, SceneManager& sceneManager)
    : MCamera(camera), MLightManager(lightManager), mSceneManager(sceneManager),
    MWireframe(false), MCursorVisible(false), MFirstMouse(true),
    MLastX(800 / 2.0f), MLastY(600 / 2.0f)
{
    MKeyState = {
        {GLFW_KEY_1, false},
        {GLFW_KEY_2, false},
        {GLFW_KEY_3, false},
        {GLFW_KEY_4, false},
        {GLFW_KEY_C, false},
        {GLFW_KEY_X, false},
		{GLFW_KEY_TAB, false}
    };
}

void InputManager::processInput(GLFWwindow* Window, const float DeltaTime)
{
    // Check current state of alt keys
    bool altCurrentlyDown = (glfwGetKey(Window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS ||
        glfwGetKey(Window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS);

    // If alt was just pressed (transition from false to true)
    if (altCurrentlyDown && !mAltDown) {
        mAltDown = true;
        glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        // Reset the cursor position to the center only once
        int width, height;
        glfwGetWindowSize(Window, &width, &height);
        glfwSetCursorPos(Window, width / 2.0, height / 2.0);
        // Optionally, do NOT reset MFirstMouse here if you want a smooth transition.
    }

    // If alt was just released (transition from true to false)
    if (!altCurrentlyDown && mAltDown) {
        mAltDown = false;
        glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        MFirstMouse = true; // so that the next movement is smooth
    }

    if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(Window, true);

    // Handle scene changes by delegating to the SceneManager
    if (glfwGetKey(Window, GLFW_KEY_1) == GLFW_PRESS)
        mSceneManager.switchScene(SceneType::SCENE_1);
    if (glfwGetKey(Window, GLFW_KEY_2) == GLFW_PRESS)
        mSceneManager.switchScene(SceneType::SCENE_2);
    if (glfwGetKey(Window, GLFW_KEY_3) == GLFW_PRESS)
        mSceneManager.switchScene(SceneType::SCENE_3);
    if (glfwGetKey(Window, GLFW_KEY_4) == GLFW_PRESS)
        mSceneManager.switchScene(SceneType::SCENE_4);

    // Handle wireframe toggle (X key)
    if (glfwGetKey(Window, GLFW_KEY_X) == GLFW_PRESS && !MKeyState[GLFW_KEY_X])
    {
        MKeyState[GLFW_KEY_X] = true;
        toggleWireframeMode();
    }
    else if (glfwGetKey(Window, GLFW_KEY_X) == GLFW_RELEASE)
    {
        MKeyState[GLFW_KEY_X] = false;
    }

    // Handle cursor visibility toggle (C key)
    if (glfwGetKey(Window, GLFW_KEY_C) == GLFW_PRESS && !MKeyState[GLFW_KEY_C])
    {
        MKeyState[GLFW_KEY_C] = true;
        toggleCursorVisibility(Window);
    }
    else if (glfwGetKey(Window, GLFW_KEY_C) == GLFW_RELEASE)
    {
        MKeyState[GLFW_KEY_C] = false;
    }

    // Movement controls (W, A, S, D, Q, E)
    if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
        MCamera.processKeyboard(Forward, DeltaTime);
    if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
        MCamera.processKeyboard(Backward, DeltaTime);
    if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
        MCamera.processKeyboard(Left, DeltaTime);
    if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
        MCamera.processKeyboard(Right, DeltaTime);
    if (glfwGetKey(Window, GLFW_KEY_Q) == GLFW_PRESS)
        MCamera.processKeyboard(Up, DeltaTime);
    if (glfwGetKey(Window, GLFW_KEY_E) == GLFW_PRESS)
        MCamera.processKeyboard(Down, DeltaTime);
}

void InputManager::toggleWireframeMode()
{
    MWireframe = !MWireframe;
    if (MWireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void InputManager::toggleCursorVisibility(GLFWwindow* Window)
{
    MCursorVisible = !MCursorVisible;
    if (MCursorVisible)
    {
        glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        MFirstMouse = true;
    }
    else
    {
        glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

void InputManager::frameBufferSizeCallback(GLFWwindow* Window, const int Width, const int Height)
{
    glViewport(0, 0, Width, Height);
}

void InputManager::mouseCallback(GLFWwindow* Window, const double PosX, const double PosY)
{
    // If the alt key is pressed, don't update the camera
    if (glfwGetKey(Window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS ||
        glfwGetKey(Window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS)
    {
        return;
    }

    // If the cursor is visible, do not process camera movement
    if (MCursorVisible) return;

    // On the first mouse movement after cursor is disabled, reset the last position
    if (MFirstMouse)
    {
        MLastX = PosX;
        MLastY = PosY;
        MFirstMouse = false;
    }

    const double OffsetX = PosX - MLastX;
    const double OffsetY = MLastY - PosY;  // y-coordinates are inverted

    MLastX = PosX;
    MLastY = PosY;

    // Update the camera's orientation based on mouse movement
    MCamera.processMouseMovement(static_cast<float>(OffsetX), static_cast<float>(OffsetY));
}

void InputManager::scrollCallback(GLFWwindow* Window, double OffsetX, const double OffsetY) const
{
    MCamera.processMouseScroll(static_cast<float>(OffsetY));
}

void InputManager::enableRawMouseMotion(GLFWwindow* window)
{
    if (glfwRawMouseMotionSupported())
    {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        std::cout << "Raw mouse motion enabled." << std::endl;
    }
    else
    {
        std::cout << "Raw mouse motion not supported on this system." << std::endl;
    }
}
