/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2025 Media Design School

File Name : Skybox.h
Description : Definitions for skybox rendering in OpenGL
Author : Ayoub Ahmad
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
	void render(const Shader& SkyboxShader, const Camera& Camera, int ScrWidth, int ScrHeight) const;
	void cleanup();

private:
	void setupSkybox();
	static unsigned int loadCubeMap(const std::vector<std::string>& Faces);

	unsigned int PvVao;
	unsigned int PvVbo;
	unsigned int PvCubeMapTexture;

	std::vector<std::string> PvFaces;
};
