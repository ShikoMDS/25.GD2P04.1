/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2025 Media Design School

File Name : Engine.h
Description : Core engine interface that manages the application loop,
	initialization, and coordinates subsystems like rendering, input,
	and scenes
Author : Ayoub Ahmad
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#pragma once
#include "Camera.h"
#include "LightManager.h"
#include "InputManager.h"
#include "SceneManager.h"

#include <glfw3.h>

class Engine
{
public:
	explicit Engine(GLFWwindow* Window);
	void run();

private:
	GLFWwindow* PvWindow;
	Camera PvCamera;
	LightManager PvLightManager;
	SceneManager PvSceneManager;
	InputManager PvInputManager;
};
