/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2025 Media Design School

File Name : main.cpp
Description : Main program entry for Assignment 1 OpenGL project
Author : Ayoub Ahmad
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#include "Engine.h"
#include <glfw3.h>
#include <glew.h>
#include <iostream>

constexpr unsigned int ScrWidth = 800;
constexpr unsigned int ScrHeight = 600;

int main()
{
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << '\n';
		return -1;
	}
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(ScrWidth, ScrHeight, "OpenGL Project", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "Failed to create GLFW window" << '\n';
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW" << '\n';
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Engine App(window);
	App.run();

	glfwTerminate();
	return 0;
}
