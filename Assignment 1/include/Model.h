/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2025 Media Design School

File Name : Model.h
Description : Definitions for object models in OpenGL
Author : Shikomisen (Ayoub Ahmad)
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#pragma once

#include "Shader.h"
#include "Mesh.h"

#include <glew.h>
#include <glm.hpp>
#include <string>
#include <vector>

class Model
{
public:
	Model(const std::string& ModelPath, const std::string& TexturePath);

	void draw(const Shader& Shader) const;
	void cleanup();

private:
	void loadModel(const std::string& Path);
	void loadTexture(const std::string& Path);

	std::vector<Mesh> PvMeshes;
	std::string PvDirectory;
	std::vector<Texture> PvTexturesLoaded;
	std::string PvTexturePath;
};

unsigned int textureFromFile(const char* Path, const std::string& Directory, bool Gamma = false);
