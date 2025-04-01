/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2024 Media Design School

File Name : Skybox.h
Description : Definitions for skybox rendering in OpenGL
Author : Shikomisen (Ayoub Ahmad)
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#pragma once

#include "Shader.h"

#include <glew.h>
#include <glm.hpp>
#include <vector>

#include "Camera.h"

class Skybox
{
public:
	Skybox();

	void draw(const Shader& Shader) const;
	void render(const Shader& skyboxShader, const Camera& camera, int scrWidth, int scrHeight) const;
	void cleanup();

private:
	void setupSkybox();
	static unsigned int loadCubeMap(const std::vector<std::string>& Faces);

	unsigned int MVao;
	unsigned int MVbo;
	unsigned int MCubeMapTexture;

	std::vector<std::string> Faces;
};
