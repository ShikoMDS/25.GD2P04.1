/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2024 Media Design School

File Name : Shader.h
Description : Definitions for shader compilation for OpenGL
Author : Shikomisen (Ayoub Ahmad)
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#pragma once

#include <glew.h>
#include <glm.hpp>
#include <string>

struct Material
{
	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;
	float Shininess;
};

struct Light
{
	glm::vec3 Position;
	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;
};

class Shader
{
public:
	Shader(const char* VertexPath, const char* FragmentPath);

	void use() const;
	void setBool(const std::string& Name, bool Value) const;
	void setInt(const std::string& Name, int Value) const;
	void setFloat(const std::string& Name, float Value) const;
	void setVec3(const std::string& Name, const glm::vec3& Value) const;
	void setVec3(const std::string& Name, float X, float Y, float Z) const;
	void setMat4(const std::string& Name, const glm::mat4& Mat) const;
	void setLight(const std::string& Name, const Light& Light) const;
	GLuint getId();
	static void checkCompileErrors(unsigned int Shader, const std::string& Type);
	static void checkLinkErrors(unsigned int Program);

	unsigned int Id;

	// Inside Shader.h
	void setMaterial(const Material& material)
	{
		setVec3("material.ambient", material.Ambient);
		setVec3("material.diffuse", material.Diffuse);
		setVec3("material.specular", material.Specular);
		setFloat("material.shininess", material.Shininess);
	}

};
