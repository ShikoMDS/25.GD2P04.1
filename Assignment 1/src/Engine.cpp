/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2025 Media Design School

File Name : Engine.cpp
Description : Implementation of the core engine functionality including
	the application loop, subsystem coordination, and resource management
Author : Ayoub Ahmad
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#include "Engine.h"

Engine::Engine(GLFWwindow* Window)
	: PvWindow(Window),
	  PvCamera(glm::vec3(0.0f, 5.0f, 30.0f)),
	  PvSceneManager(PvCamera, PvLightManager),
	  PvInputManager(PvCamera, PvSceneManager)
{
	glfwSetInputMode(PvWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetWindowUserPointer(PvWindow, this);

	glfwSetCursorPosCallback(PvWindow, [](GLFWwindow* Win, const double Xpos, const double Ypos)
	{
		const auto AppEngine = static_cast<Engine*>(glfwGetWindowUserPointer(Win));
		AppEngine->PvInputManager.mouseCallback(Win, Xpos, Ypos);
	});

	glfwSetScrollCallback(PvWindow, [](GLFWwindow* Win, const double Xoffset, const double Yoffset)
	{
		const auto AppEngine = static_cast<Engine*>(glfwGetWindowUserPointer(Win));
		AppEngine->PvInputManager.scrollCallback(Win, Xoffset, Yoffset);
	});

	glfwSetFramebufferSizeCallback(PvWindow, [](GLFWwindow* Win, const int Width, const int Height)
	{
		[[maybe_unused]] const auto AppEngine = static_cast<Engine*>(glfwGetWindowUserPointer(Win));
		InputManager::frameBufferSizeCallback(Win, Width, Height);
	});
}

void Engine::run()
{
	float LastFrame = 0.0f;
	while (!glfwWindowShouldClose(PvWindow))
	{
		const float CurrentFrame = static_cast<float>(glfwGetTime());
		const float DeltaTime = CurrentFrame - LastFrame;
		LastFrame = CurrentFrame;

		PvInputManager.processInput(PvWindow, DeltaTime);
		PvSceneManager.update(DeltaTime);
		PvSceneManager.render();

		glfwSwapBuffers(PvWindow);
		glfwPollEvents();
	}
	PvSceneManager.cleanup();
}
