/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2024 Media Design School

File Name : LightManager.h
Description : Definitions for lighting in OpenGL
Author : Shikomisen (Ayoub Ahmad)
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#pragma once

#include "Shader.h"

#include <glm.hpp>
#include <string>

struct PointLight
{
	glm::vec3 Position;
	glm::vec3 Colour;
	float Constant;
	float Linear;
	float Quadratic;
};

struct DirectionalLight
{
	glm::vec3 Direction;
	glm::vec3 Colour;
	float AmbientStrength;
};


struct SpotLight
{
	glm::vec3 Position;
	glm::vec3 Direction;
	glm::vec3 Colour;
	float CutOff;
	float OuterCutOff;
	float Constant;
	float Linear;
	float Quadratic;
};

class LightManager
{
public:
	LightManager();

	void initialize();
	void updateLighting(const Shader& Shader) const;

	void togglePointLights();
	void toggleDirectionalLight();
	void toggleSpotLight();

	void setSpotLightPosition(const glm::vec3& Position);
	void setSpotLightDirection(const glm::vec3& Direction);
	[[nodiscard]] SpotLight getSpotLight() const;

	[[nodiscard]] bool isPointLightsOn() const;
	[[nodiscard]] PointLight& getPointLight(int Index);

private:
	PointLight MPointLights[10];
	DirectionalLight MDirectionalLight;
	SpotLight MSpotLight;

	bool MPointLightsOn;
	bool MDirectionalLightOn;
	bool MSpotLightOn;
};
