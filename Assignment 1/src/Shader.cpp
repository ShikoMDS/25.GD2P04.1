/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2024 Media Design School

File Name : Shader.cpp
Description : Implementations for Shader class
Author : Shikomisen (Ayoub Ahmad)
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    // 1. Retrieve shader source code
    std::string vertexCode, fragmentCode;
    std::ifstream vShaderFile, fShaderFile;

    // Ensure ifstream objects can throw exceptions
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
        std::cerr << "Exception message: " << e.what() << std::endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. Compile shaders
    unsigned int vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // 3. Link shaders to program
    Id = glCreateProgram();
    glAttachShader(Id, vertex);
    glAttachShader(Id, fragment);
    glLinkProgram(Id);
    checkLinkErrors(Id);

    // 4. Delete shaders as they're linked now
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() const
{
	glUseProgram(Id);
}

void Shader::setBool(const std::string& Name, const bool Value) const
{
	glUniform1i(glGetUniformLocation(Id, Name.c_str()), static_cast<int>(Value));
}

void Shader::setInt(const std::string& Name, const int Value) const
{
	glUniform1i(glGetUniformLocation(Id, Name.c_str()), Value);
}

void Shader::setFloat(const std::string& Name, const float Value) const
{
	glUniform1f(glGetUniformLocation(Id, Name.c_str()), Value);
}

void Shader::setVec3(const std::string& Name, const glm::vec3& Value) const
{
	glUniform3fv(glGetUniformLocation(Id, Name.c_str()), 1, &Value[0]);
}

void Shader::setVec3(const std::string& Name, const float X, const float Y, const float Z) const
{
	glUniform3f(glGetUniformLocation(Id, Name.c_str()), X, Y, Z);
}

void Shader::setMat4(const std::string& Name, const glm::mat4& Mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(Id, Name.c_str()), 1, GL_FALSE, &Mat[0][0]);
}

void Shader::setLight(const std::string& Name, const Light& Light) const
{
	setVec3(Name + ".position", Light.Position);
	setVec3(Name + ".ambient", Light.Ambient);
	setVec3(Name + ".diffuse", Light.Diffuse);
	setVec3(Name + ".specular", Light.Specular);
}

GLuint Shader::getId()
{
    return Id;
}

void Shader::checkCompileErrors(const unsigned int Shader, const std::string& Type)
{
	int Success;
	char InfoLog[1024];
	if (Type != "PROGRAM")
	{
		glGetShaderiv(Shader, GL_COMPILE_STATUS, &Success);
		if (!Success)
		{
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
	char InfoLog[1024];
	glGetProgramiv(Program, GL_LINK_STATUS, &Success);
	if (!Success)
	{
		glGetProgramInfoLog(Program, 1024, nullptr, InfoLog);
		std::cout << "ERROR::PROGRAM_LINKING_ERROR\n" << InfoLog <<
			"\n -- --------------------------------------------------- -- " <<
			'\n';
	}
}
