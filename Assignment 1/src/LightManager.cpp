/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2024 Media Design School

File Name : LightManager.cpp
Description : Implementations for LightManager class
Author : Shikomisen (Ayoub Ahmad)
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#include "LightManager.h"

#include "Camera.h"
#include "Camera.h"
#include "Camera.h"
#include "Camera.h"

LightManager::LightManager() = default;

void LightManager::initialize()
{
	MPointLights[0] = {glm::vec3(-2.0f, 0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 1.0f, 0.09f, 0.032f}; // Red light
	MPointLights[1] = {glm::vec3(2.0f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 1.0f, 0.09f, 0.032f}; // Blue light
	MDirectionalLight = {glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.4f, 0.4f, 0.4f), 0.1f};
	MSpotLight = {
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f),
		glm::cos(glm::radians(20.0f)), glm::cos(glm::radians(25.0f)), 1.0f, 0.09f, 0.032f
	};

	MSpotLightOn = true;
	MPointLightsOn = true;
	MDirectionalLightOn = true;
}

void LightManager::updateLighting(const Shader& Shader) const
{
	Shader.setVec3("directionalLight.direction", MDirectionalLight.Direction);
	Shader.setVec3("directionalLight.color", MDirectionalLight.Colour);
	Shader.setFloat("directionalLight.ambientStrength", MDirectionalLight.AmbientStrength);

	for (int I = 0; I < 2; I++)
	{
		Shader.setVec3("pointLights[" + std::to_string(I) + "].position", MPointLights[I].Position);
		Shader.setVec3("pointLights[" + std::to_string(I) + "].color", MPointLights[I].Colour);
		Shader.setFloat("pointLights[" + std::to_string(I) + "].constant", MPointLights[I].Constant);
		Shader.setFloat("pointLights[" + std::to_string(I) + "].linear", MPointLights[I].Linear);
		Shader.setFloat("pointLights[" + std::to_string(I) + "].quadratic", MPointLights[I].Quadratic);
	}

	Shader.setVec3("spotLight.position", MSpotLight.Position);
	Shader.setVec3("spotLight.direction", MSpotLight.Direction);
	Shader.setVec3("spotLight.color", MSpotLight.Colour);
	Shader.setFloat("spotLight.cutOff", MSpotLight.CutOff);
	Shader.setFloat("spotLight.outerCutOff", MSpotLight.OuterCutOff);
	Shader.setFloat("spotLight.constant", MSpotLight.Constant);
	Shader.setFloat("spotLight.linear", MSpotLight.Linear);
	Shader.setFloat("spotLight.quadratic", MSpotLight.Quadratic);
}

void LightManager::togglePointLights()
{
	MPointLightsOn = !MPointLightsOn;

	if (!MPointLightsOn)
	{
		MPointLights[0].Colour = glm::vec3(0.0f);
		MPointLights[1].Colour = glm::vec3(0.0f);
	}
	else
	{
		MPointLights[0].Colour = glm::vec3(1.0f, 0.0f, 0.0f); // Red light
		MPointLights[1].Colour = glm::vec3(0.0f, 0.0f, 1.0f); // Blue light
	}
}

void LightManager::toggleDirectionalLight()
{
	MDirectionalLightOn = !MDirectionalLightOn;
	MDirectionalLight.Colour = MDirectionalLightOn ? glm::vec3(0.4f, 0.4f, 0.4f) : glm::vec3(0.0f);
}

void LightManager::toggleSpotLight()
{
	MSpotLightOn = !MSpotLightOn;
	MSpotLight.Colour = MSpotLightOn ? glm::vec3(1.0f, 1.0f, 1.0f) : glm::vec3(0.0f);
}

void LightManager::setSpotLightPosition(const glm::vec3& Position)
{
	MSpotLight.Position = Position;
}

void LightManager::setSpotLightDirection(const glm::vec3& Direction)
{
	MSpotLight.Direction = Direction;
}

SpotLight LightManager::getSpotLight() const
{
	return MSpotLight;
}

bool LightManager::isPointLightsOn() const
{
	return MPointLightsOn;
}

PointLight& LightManager::getPointLight(int Index) 
{
	return MPointLights[Index];
}
