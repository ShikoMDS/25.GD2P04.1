/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2025 Media Design School

File Name : Scene.h
Description : Base abstract class for all scenes in the rendering engine
Author : Ayoub Ahmad
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#pragma once

enum class SceneType
{
	Scene1,
	Scene2,
	Scene3,
	Scene4
};

class Scene
{
public:
	virtual void load() = 0;
	virtual void update(float DeltaTime) = 0;
	virtual void render() = 0;
	virtual void cleanup() = 0;
	virtual ~Scene() = default;

protected:
	Scene() = default;
	Scene(const Scene&) = default;
	Scene& operator=(const Scene&) = default;
	Scene(Scene&&) = default;
	Scene& operator=(Scene&&) = default;
};
