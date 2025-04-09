/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2025 Media Design School

File Name : SceneManager.h
Description : Declarations for scene management and switching in OpenGL
Author : Ayoub Ahmad
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#pragma once
#include "Scene.h"
#include "Camera.h"
#include "LightManager.h"

#include <memory>

class SceneManager
{
public:
	SceneManager(Camera& Camera, LightManager& LightManager);

	void switchScene(SceneType NewScene);

	void resetCamera() const;

	void update(float DeltaTime) const;
	void render() const;

	void cleanup();

private:
	std::unique_ptr<Scene> PvCurrentScene;
	SceneType PvActiveScene;
	Camera* PvCamera;
	LightManager* PvLightManager;
};
