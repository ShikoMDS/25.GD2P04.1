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

LightManager::LightManager() = default;

void LightManager::initialize()
{
	PvPointLights[0] = {glm::vec3(-2.0f, 0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 1.0f, 0.09f, 0.032f}; // Red light
	PvPointLights[1] = {glm::vec3(2.0f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 1.0f, 0.09f, 0.032f}; // Blue light
	PvDirectionalLight = {glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.4f, 0.4f, 0.4f), 0.1f};
	PvSpotLight = {
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f),
		glm::cos(glm::radians(20.0f)), glm::cos(glm::radians(25.0f)), 1.0f, 0.09f, 0.032f
	};

	PvSpotLightOn = true;
	PvPointLightsOn = true;
	PvDirectionalLightOn = true;
}

void LightManager::updateLighting(const Shader& Shader) const
{
	Shader.setVec3("directionalLight.direction", PvDirectionalLight.Direction);
	Shader.setVec3("directionalLight.color", PvDirectionalLight.Colour);
	Shader.setFloat("directionalLight.ambientStrength", PvDirectionalLight.AmbientStrength);

	for (int I = 0; I < 2; I++)
	{
		Shader.setVec3("pointLights[" + std::to_string(I) + "].position", PvPointLights[I].Position);
		Shader.setVec3("pointLights[" + std::to_string(I) + "].color", PvPointLights[I].Colour);
		Shader.setFloat("pointLights[" + std::to_string(I) + "].constant", PvPointLights[I].Constant);
		Shader.setFloat("pointLights[" + std::to_string(I) + "].linear", PvPointLights[I].Linear);
		Shader.setFloat("pointLights[" + std::to_string(I) + "].quadratic", PvPointLights[I].Quadratic);
	}

	Shader.setVec3("spotLight.position", PvSpotLight.Position);
	Shader.setVec3("spotLight.direction", PvSpotLight.Direction);
	Shader.setVec3("spotLight.color", PvSpotLight.Colour);
	Shader.setFloat("spotLight.cutOff", PvSpotLight.CutOff);
	Shader.setFloat("spotLight.outerCutOff", PvSpotLight.OuterCutOff);
	Shader.setFloat("spotLight.constant", PvSpotLight.Constant);
	Shader.setFloat("spotLight.linear", PvSpotLight.Linear);
	Shader.setFloat("spotLight.quadratic", PvSpotLight.Quadratic);
}

void LightManager::togglePointLights()
{
	PvPointLightsOn = !PvPointLightsOn;

	if (!PvPointLightsOn)
	{
		PvPointLights[0].Colour = glm::vec3(0.0f);
		PvPointLights[1].Colour = glm::vec3(0.0f);
	}
	else
	{
		PvPointLights[0].Colour = glm::vec3(1.0f, 0.0f, 0.0f); // Red light
		PvPointLights[1].Colour = glm::vec3(0.0f, 0.0f, 1.0f); // Blue light
	}
}

void LightManager::toggleDirectionalLight()
{
	PvDirectionalLightOn = !PvDirectionalLightOn;
	PvDirectionalLight.Colour = PvDirectionalLightOn ? glm::vec3(0.4f, 0.4f, 0.4f) : glm::vec3(0.0f);
}

void LightManager::toggleSpotLight()
{
	PvSpotLightOn = !PvSpotLightOn;
	PvSpotLight.Colour = PvSpotLightOn ? glm::vec3(1.0f, 1.0f, 1.0f) : glm::vec3(0.0f);
}

void LightManager::setSpotLightPosition(const glm::vec3& Position)
{
	PvSpotLight.Position = Position;
}

void LightManager::setSpotLightDirection(const glm::vec3& Direction)
{
	PvSpotLight.Direction = Direction;
}

SpotLight LightManager::getSpotLight() const
{
	return PvSpotLight;
}

bool LightManager::isPointLightsOn() const
{
	return PvPointLightsOn;
}

PointLight& LightManager::getPointLight(int Index)
{
	return PvPointLights[Index];
}
