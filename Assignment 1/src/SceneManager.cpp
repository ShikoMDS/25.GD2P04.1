/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2025 Media Design School

File Name : SceneManager.cpp
Description : Implementations for Scene Manager class
Author : Ayoub Ahmad
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#include "SceneManager.h"
#include "Scene1.h"
#include "Scene2.h"
#include "Scene3.h"
#include "Scene4.h"

#include <iostream>

SceneManager::SceneManager(Camera& Camera, LightManager& LightManager)
	: PvActiveScene(SceneType::Scene1), PvCamera(&Camera), PvLightManager(&LightManager)
{
	switchScene(SceneType::Scene1);
}

void SceneManager::resetCamera() const
{
	switch (PvActiveScene)
	{
	case SceneType::Scene1:
		PvCamera->PbPosition = glm::vec3(0.0f, 5.0f, 15.0f);
		PvCamera->PbFront = glm::vec3(0.0f, 0.0f, -1.0f);
		PvCamera->PbUp = glm::vec3(0.0f, 1.0f, 0.0f);
		PvCamera->PbYaw = -90.0f;
		PvCamera->PbPitch = -5.0f;
		break;
	case SceneType::Scene2:
		PvCamera->PbPosition = glm::vec3(0.0f, 10.0f, 35.0f);
		PvCamera->PbFront = glm::vec3(0.0f, -0.5f, -1.0f);
		PvCamera->PbUp = glm::vec3(0.0f, 1.0f, 0.0f);
		PvCamera->PbYaw = -90.0f;
		PvCamera->PbPitch = -20.0f;
		break;
	case SceneType::Scene3:
		PvCamera->PbPosition = glm::vec3(0.0f, 0.0f, 3.0f);
		PvCamera->PbFront = glm::vec3(0.0f, 0.0f, -1.0f);
		PvCamera->PbUp = glm::vec3(0.0f, 1.0f, 0.0f);
		PvCamera->PbYaw = -90.0f;
		PvCamera->PbPitch = 0.0f;
		break;
	case SceneType::Scene4:
		PvCamera->PbPosition = glm::vec3(-1.0f, 5.0f, 32.5f);
		PvCamera->PbFront = glm::vec3(0.0f, 0.0f, -1.0f);
		PvCamera->PbUp = glm::vec3(0.0f, 1.0f, 0.0f);
		PvCamera->PbYaw = -90.0f;
		PvCamera->PbPitch = 0.0f;
		break;
	}

	PvCamera->updateCameraVectors();
}

void SceneManager::switchScene(SceneType NewScene)
{
	if (!PvCurrentScene || PvActiveScene != NewScene)
	{
		if (PvCurrentScene)
		{
			std::cout << "Cleaning up current scene..." << '\n';
			PvCurrentScene->cleanup();
			PvCurrentScene.reset();
		}

		std::cout << "Switching to scene type " << static_cast<int>(NewScene) << '\n';

		PvActiveScene = NewScene;

		resetCamera();

		try
		{
			switch (NewScene)
			{
			case SceneType::Scene1:
				PvCurrentScene = std::make_unique<Scene1>(*PvCamera, *PvLightManager);
				break;
			case SceneType::Scene2:
				PvCurrentScene = std::make_unique<Scene2>(*PvCamera, *PvLightManager);
				break;
			case SceneType::Scene3:
				PvCurrentScene = std::make_unique<Scene3>();
				break;
			case SceneType::Scene4:
				PvCurrentScene = std::make_unique<Scene4>(*PvCamera, *PvLightManager);
				break;
			}

			PvCurrentScene->load();
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error switching to scene: " << e.what() << '\n';
			if (NewScene != SceneType::Scene1)
			{
				std::cerr << "Falling back to Scene 1" << '\n';
				PvCurrentScene = std::make_unique<Scene1>(*PvCamera, *PvLightManager);
				PvCurrentScene->load();
				PvActiveScene = SceneType::Scene1;
				resetCamera();
			}
		}
	}
	else
	{
		std::cout << "Already in the active scene. No need to switch." << '\n';
	}
}

void SceneManager::update(const float DeltaTime) const
{
	if (PvCurrentScene)
		PvCurrentScene->update(DeltaTime);
}

void SceneManager::render() const
{
	if (PvCurrentScene)
		PvCurrentScene->render();
}

void SceneManager::cleanup()
{
	if (PvCurrentScene)
	{
		PvCurrentScene->cleanup();
		PvCurrentScene.reset();
	}
}
