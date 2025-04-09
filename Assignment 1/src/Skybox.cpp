/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2025 Media Design School

File Name : Skybox.cpp
Description : Implementations for Skybox class
Author : Ayoub Ahmad
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#include "Skybox.h"
// #DEFINE FOR STB IMAGE HAS ALREADY BEEN DECLARED
#include "stb_image.h"

#include <iostream>

Skybox::Skybox()
{
	PvFaces = {
		"resources/skybox/Corona/Right.png",
		"resources/skybox/Corona/Left.png",
		"resources/skybox/Corona/Top.png",
		"resources/skybox/Corona/Bottom.png",
		"resources/skybox/Corona/Back.png",
		"resources/skybox/Corona/Front.png"
	};

	PvCubeMapTexture = loadCubeMap(PvFaces);
	setupSkybox();
}

void Skybox::draw(const Shader& Shader) const
{
	glBindVertexArray(PvVao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, PvCubeMapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void Skybox::render(const Shader& SkyboxShader, const Camera& Camera, int ScrWidth, int ScrHeight) const
{
	glDepthFunc(GL_LEQUAL);
	SkyboxShader.use();
	SkyboxShader.setMat4("view", glm::mat4(glm::mat3(Camera.getViewMatrix())));
	SkyboxShader.setMat4("projection",
	                     Camera.getProjectionMatrix(static_cast<float>(ScrWidth), static_cast<float>(ScrHeight)));
	draw(SkyboxShader);
	glDepthFunc(GL_LESS);
}

void Skybox::cleanup()
{
	//std::cout << "Cleaning up Skybox resources..." << '\n';

	// Clean up VAO
	if (PvVao != 0)
	{
		glDeleteVertexArrays(1, &PvVao);
		PvVao = 0;
	}

	// Clean up the cube map texture
	if (PvCubeMapTexture != 0)
	{
		glDeleteTextures(1, &PvCubeMapTexture);
		PvCubeMapTexture = 0;
	}
}

void Skybox::setupSkybox()
{
	constexpr float SkyboxVertices[] = {
		// Positions          
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f
	};

	glGenVertexArrays(1, &PvVao);
	glGenBuffers(1, &PvVbo);
	glBindVertexArray(PvVao);
	glBindBuffer(GL_ARRAY_BUFFER, PvVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SkyboxVertices), &SkyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
	glBindVertexArray(0);
}

unsigned int Skybox::loadCubeMap(const std::vector<std::string>& Faces)
{
	unsigned int TextureId;
	glGenTextures(1, &TextureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureId);

	int Width, Height, NrComponents;
	for (unsigned int I = 0; I < Faces.size(); I++)
	{
		if (unsigned char* Data = stbi_load(Faces[I].c_str(), &Width, &Height, &NrComponents, 0))
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + I,
			             0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Data
			);
			stbi_image_free(Data);
		}
		else
		{
			std::cerr << "Cube map texture failed to load at path: " << Faces[I] << '\n';
			stbi_image_free(Data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return TextureId;
}
