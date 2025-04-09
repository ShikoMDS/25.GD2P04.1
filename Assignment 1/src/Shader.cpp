/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2025 Media Design School

File Name : Shader.cpp
Description : Implementations for Shader class
Author : Shikomisen (Ayoub Ahmad)
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const char* VertexPath, const char* FragmentPath)
{
	std::string VertexCode, FragmentCode;
	std::ifstream VShaderFile, FShaderFile;

	VShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	FShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		VShaderFile.open(VertexPath);
		FShaderFile.open(FragmentPath);
		std::stringstream VShaderStream, FShaderStream;
		VShaderStream << VShaderFile.rdbuf();
		FShaderStream << FShaderFile.rdbuf();
		VShaderFile.close();
		FShaderFile.close();
		VertexCode = VShaderStream.str();
		FragmentCode = FShaderStream.str();
	}
	catch (std::ifstream::failure& E)
	{
		std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << '\n';
		std::cerr << "Exception message: " << E.what() << '\n';
	}

	const char* VShaderCode = VertexCode.c_str();
	const char* FShaderCode = FragmentCode.c_str();

	unsigned int Vertex, Fragment;
	Vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(Vertex, 1, &VShaderCode, nullptr);
	glCompileShader(Vertex);
	checkCompileErrors(Vertex, "VERTEX");

	Fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(Fragment, 1, &FShaderCode, nullptr);
	glCompileShader(Fragment);
	checkCompileErrors(Fragment, "FRAGMENT");

	PbId = glCreateProgram();
	glAttachShader(PbId, Vertex);
	glAttachShader(PbId, Fragment);
	glLinkProgram(PbId);
	checkLinkErrors(PbId);

	glDeleteShader(Vertex);
	glDeleteShader(Fragment);
}

void Shader::use() const
{
	glUseProgram(PbId);
}

void Shader::setBool(const std::string& Name, const bool Value) const
{
	glUniform1i(glGetUniformLocation(PbId, Name.c_str()), static_cast<int>(Value));
}

void Shader::setInt(const std::string& Name, const int Value) const
{
	glUniform1i(glGetUniformLocation(PbId, Name.c_str()), Value);
}

void Shader::setFloat(const std::string& Name, const float Value) const
{
	glUniform1f(glGetUniformLocation(PbId, Name.c_str()), Value);
}

void Shader::setVec3(const std::string& Name, const glm::vec3& Value) const
{
	glUniform3fv(glGetUniformLocation(PbId, Name.c_str()), 1, &Value[0]);
}

void Shader::setVec3(const std::string& Name, const float X, const float Y, const float Z) const
{
	glUniform3f(glGetUniformLocation(PbId, Name.c_str()), X, Y, Z);
}

void Shader::setMat4(const std::string& Name, const glm::mat4& Mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(PbId, Name.c_str()), 1, GL_FALSE, &Mat[0][0]);
}

void Shader::setLight(const std::string& Name, const Light& Light) const
{
	setVec3(Name + ".position", Light.Position);
	setVec3(Name + ".ambient", Light.Ambient);
	setVec3(Name + ".diffuse", Light.Diffuse);
	setVec3(Name + ".specular", Light.Specular);
}

GLuint Shader::getId() const
{
	return PbId;
}

void Shader::checkCompileErrors(const unsigned int Shader, const std::string& Type)
{
	int Success;
	if (Type != "PROGRAM")
	{
		glGetShaderiv(Shader, GL_COMPILE_STATUS, &Success);
		if (!Success)
		{
			char InfoLog[1024];
			glGetShaderInfoLog(Shader, 1024, nullptr, InfoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << Type << "\n" << InfoLog <<
				"\n -- --------------------------------------------------- -- " <<
				'\n';
		}
	}
}

void Shader::checkLinkErrors(const unsigned int Program)
{
	int Success;
	glGetProgramiv(Program, GL_LINK_STATUS, &Success);
	if (!Success)
	{
		char InfoLog[1024];
		glGetProgramInfoLog(Program, 1024, nullptr, InfoLog);
		std::cout << "ERROR::PROGRAM_LINKING_ERROR\n" << InfoLog <<
			"\n -- --------------------------------------------------- -- " <<
			'\n';
	}
}
