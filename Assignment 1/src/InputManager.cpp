/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2025 Media Design School

File Name : InputManager.cpp
Description : Implementations for InputManager class handling user input
Author : Ayoub Ahmad
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#include "InputManager.h"
#include "SceneManager.h"

#include <iostream>
#include <glfw3.h>

InputManager::InputManager(Camera& Camera, SceneManager& SceneManager)
	: PvCamera(&Camera), PvSceneManager(&SceneManager),
	  PvWireframe(false), PvCursorVisible(false), PvFirstMouse(true),
	  PvLastX(800 / 2.0f), PvLastY(600 / 2.0f)
{
	PvKeyState = {
		{GLFW_KEY_1, false},
		{GLFW_KEY_2, false},
		{GLFW_KEY_3, false},
		{GLFW_KEY_4, false},
		{GLFW_KEY_C, false},
		{GLFW_KEY_X, false},
		{GLFW_KEY_R, false},
		{GLFW_KEY_TAB, false}
	};
}

void InputManager::processInput(GLFWwindow* Window, const float DeltaTime)
{
	// Check current state of alt keys
	const bool AltCurrentlyDown = (glfwGetKey(Window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS ||
		glfwGetKey(Window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS);

	// If alt was just pressed (transition from false to true)
	if (AltCurrentlyDown && !PvAltDown)
	{
		PvAltDown = true;
		glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Reset the cursor position to the center only once
		int Width, Height;
		glfwGetWindowSize(Window, &Width, &Height);
		glfwSetCursorPos(Window, Width / 2.0, Height / 2.0);
	}

	// If alt was just released (transition from true to false)
	if (!AltCurrentlyDown && PvAltDown)
	{
		PvAltDown = false;
		glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		PvFirstMouse = true;
	}

	if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(Window, true);

	// Handle scene changes with key debouncing
	if (glfwGetKey(Window, GLFW_KEY_1) == GLFW_PRESS && !PvKeyState[GLFW_KEY_1])
	{
		PvKeyState[GLFW_KEY_1] = true;
		PvSceneManager->switchScene(SceneType::Scene1);
	}
	else if (glfwGetKey(Window, GLFW_KEY_1) == GLFW_RELEASE)
	{
		PvKeyState[GLFW_KEY_1] = false;
	}

	if (glfwGetKey(Window, GLFW_KEY_2) == GLFW_PRESS && !PvKeyState[GLFW_KEY_2])
	{
		PvKeyState[GLFW_KEY_2] = true;
		PvSceneManager->switchScene(SceneType::Scene2);
	}
	else if (glfwGetKey(Window, GLFW_KEY_2) == GLFW_RELEASE)
	{
		PvKeyState[GLFW_KEY_2] = false;
	}

	if (glfwGetKey(Window, GLFW_KEY_3) == GLFW_PRESS && !PvKeyState[GLFW_KEY_3])
	{
		PvKeyState[GLFW_KEY_3] = true;
		PvSceneManager->switchScene(SceneType::Scene3);
	}
	else if (glfwGetKey(Window, GLFW_KEY_3) == GLFW_RELEASE)
	{
		PvKeyState[GLFW_KEY_3] = false;
	}

	if (glfwGetKey(Window, GLFW_KEY_4) == GLFW_PRESS && !PvKeyState[GLFW_KEY_4])
	{
		PvKeyState[GLFW_KEY_4] = true;
		PvSceneManager->switchScene(SceneType::Scene4);
	}
	else if (glfwGetKey(Window, GLFW_KEY_4) == GLFW_RELEASE)
	{
		PvKeyState[GLFW_KEY_4] = false;
	}

	// Handle wireframe toggle (X key)
	if (glfwGetKey(Window, GLFW_KEY_X) == GLFW_PRESS && !PvKeyState[GLFW_KEY_X])
	{
		PvKeyState[GLFW_KEY_X] = true;
		toggleWireframeMode();
	}
	else if (glfwGetKey(Window, GLFW_KEY_X) == GLFW_RELEASE)
	{
		PvKeyState[GLFW_KEY_X] = false;
	}

	// Handle cursor visibility toggle (C key)
	if (glfwGetKey(Window, GLFW_KEY_C) == GLFW_PRESS && !PvKeyState[GLFW_KEY_C])
	{
		PvKeyState[GLFW_KEY_C] = true;
		toggleCursorVisibility(Window);
	}
	else if (glfwGetKey(Window, GLFW_KEY_C) == GLFW_RELEASE)
	{
		PvKeyState[GLFW_KEY_C] = false;
	}

	// Reset camera position with R key
	if (glfwGetKey(Window, GLFW_KEY_R) == GLFW_PRESS && !PvKeyState[GLFW_KEY_R])
	{
		PvKeyState[GLFW_KEY_R] = true;
		PvSceneManager->resetCamera();
		std::cout << "Camera position reset for current scene" << std::endl;
	}
	else if (glfwGetKey(Window, GLFW_KEY_R) == GLFW_RELEASE)
	{
		PvKeyState[GLFW_KEY_R] = false;
	}

	// Movement controls (W, A, S, D, Q, E)
	if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
		PvCamera->processKeyboard(Forward, DeltaTime);
	if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
		PvCamera->processKeyboard(Backward, DeltaTime);
	if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
		PvCamera->processKeyboard(Left, DeltaTime);
	if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
		PvCamera->processKeyboard(Right, DeltaTime);
	if (glfwGetKey(Window, GLFW_KEY_Q) == GLFW_PRESS)
		PvCamera->processKeyboard(Up, DeltaTime);
	if (glfwGetKey(Window, GLFW_KEY_E) == GLFW_PRESS)
		PvCamera->processKeyboard(Down, DeltaTime);
}

void InputManager::toggleWireframeMode()
{
	PvWireframe = !PvWireframe;
	if (PvWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void InputManager::toggleCursorVisibility(GLFWwindow* Window)
{
	PvCursorVisible = !PvCursorVisible;
	if (PvCursorVisible)
	{
		glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		PvFirstMouse = true;
	}
	else
	{
		glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
}

void InputManager::frameBufferSizeCallback([[maybe_unused]] GLFWwindow* Window, const int Width, const int Height)
{
	glViewport(0, 0, Width, Height);
}

void InputManager::mouseCallback(GLFWwindow* Window, const double PosX, const double PosY)
{
	// If alt key is pressed, don't update camera
	if (glfwGetKey(Window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS ||
		glfwGetKey(Window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS)
	{
		return;
	}

	// If cursor is visible, do not process camera movement
	if (PvCursorVisible) return;

	// On first mouse movement after cursor is disabled, reset last position
	if (PvFirstMouse)
	{
		PvLastX = PosX;
		PvLastY = PosY;
		PvFirstMouse = false;
	}

	const double OffsetX = PosX - PvLastX;
	const double OffsetY = PvLastY - PosY;

	PvLastX = PosX;
	PvLastY = PosY;

	// Update camera's orientation based on mouse movement
	PvCamera->processMouseMovement(static_cast<float>(OffsetX), static_cast<float>(OffsetY));
}

void InputManager::scrollCallback(GLFWwindow* Window, double OffsetX, const double OffsetY) const
{
	PvCamera->processMouseScroll(static_cast<float>(OffsetY));
}

void InputManager::enableRawMouseMotion(GLFWwindow* Window)
{
	if (glfwRawMouseMotionSupported())
	{
		glfwSetInputMode(Window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		std::cout << "Raw mouse motion enabled." << '\n';
	}
	else
	{
		std::cout << "Raw mouse motion not supported on this system." << '\n';
	}
}
