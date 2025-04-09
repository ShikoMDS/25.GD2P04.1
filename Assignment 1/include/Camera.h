/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2025 Media Design School

File Name : Camera.h
Description : Definitions for Camera implementation in OpenGL
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

inline constexpr float Yaw = -90.0f;
inline constexpr float Pitch = 0.0f;
inline constexpr float Speed = 2.5f;
inline constexpr float Sensitivity = 0.05f;
inline constexpr float Zoom = 45.0f;

class Camera
{
public:
	Camera(float PosX, float PosY, float PosZ, float UpX, float UpY, float UpZ, float InputYaw, float InputPitch);
	explicit Camera(glm::vec3 Pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f),
	                float InputYaw = Yaw, float InputPitch = Pitch);

	[[nodiscard]] glm::mat4 getViewMatrix() const;
	[[nodiscard]] glm::mat4 getProjectionMatrix(float Width, float Height) const;

	void processKeyboard(CameraMovement Direction, float DeltaTime);
	void processMouseMovement(float OffsetX, float OffsetY, GLboolean ConstrainPitch = true);
	void processMouseScroll(float OffsetY);

	void reset();

	void updateCameraVectors();

	glm::vec3 PbPosition;
	glm::vec3 PbFront;
	glm::vec3 PbUp;
	glm::vec3 PbRight;
	glm::vec3 PbWorldUp;
	float PbYaw;
	float PbPitch;
	float PbMovementSpeed;
	float PbMouseSensitivity;
	float PbZoom;
};
