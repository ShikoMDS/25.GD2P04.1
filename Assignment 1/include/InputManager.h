/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2025 Media Design School

File Name : InputManager.h
Description : Declarations for window input in OpenGL
Author : Shikomisen (Ayoub Ahmad)
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#pragma once

#include "Camera.h"
#include "LightManager.h"

#include <glew.h>
#include <glfw3.h>
#include <unordered_map>

class SceneManager;

class InputManager
{
public:
	InputManager(Camera& Camera, SceneManager& SceneManager);

	void processInput(GLFWwindow* Window, float DeltaTime);
	static void frameBufferSizeCallback(GLFWwindow* Window, int Width, int Height);
	void mouseCallback(GLFWwindow* Window, double PosX, double PosY);
	void scrollCallback(GLFWwindow* Window, double OffsetX, double OffsetY) const;
	static void enableRawMouseMotion(GLFWwindow* Window);
	void toggleWireframeMode();
	void toggleCursorVisibility(GLFWwindow* Window);

private:
	Camera* PvCamera;
	SceneManager* PvSceneManager;

	bool PvWireframe;
	bool PvCursorVisible;
	bool PvFirstMouse;

	double PvLastX;
	double PvLastY;

	std::unordered_map<int, bool> PvKeyState;

	bool PvAltDown = false;
};
