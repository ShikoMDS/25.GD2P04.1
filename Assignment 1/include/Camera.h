/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2024 Media Design School

File Name : Camera.h
Description : Definitions for camera in OpenGL
Author : Shikomisen (Ayoub Ahmad)
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#pragma once

#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <vector>

enum CameraMovement
{
	Forward,
	Backward,
	Left,
	Right,
	Up,
	Down
};

constexpr float Yaw = -90.0f;
constexpr float Pitch = 0.0f;
constexpr float Speed = 2.5f;
constexpr float Sensitivity = 0.05f;
constexpr float Zoom = 45.0f;

class Camera
{
public:
	Camera(float PosX, float PosY, float PosZ, float UpX, float UpY, float UpZ, float Yaw, float Pitch);
	explicit Camera(glm::vec3 Pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f),
	                float Yaw = Yaw, float Pitch = Pitch);

	[[nodiscard]] glm::mat4 getViewMatrix() const;
	[[nodiscard]] glm::mat4 getProjectionMatrix(float Width, float Height) const;

	void processKeyboard(CameraMovement Direction, float DeltaTime);
	void processMouseMovement(float OffsetX, float OffsetY, GLboolean ConstrainPitch = true);
	void processMouseScroll(float OffsetY);

	glm::vec3 VPosition;
	glm::vec3 VFront;
	glm::vec3 VUp;
	glm::vec3 VRight;
	glm::vec3 VWorldUp;
	float FYaw;
	float FPitch;
	float FMovementSpeed;
	float FMouseSensitivity;
	float FZoom;

private:
	void updateCameraVectors();
};
