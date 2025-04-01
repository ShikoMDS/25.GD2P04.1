/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2024 Media Design School

File Name : Camera.cpp
Description : Implementations for Camera class
Author : Shikomisen (Ayoub Ahmad)
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#include "Camera.h"

Camera::Camera(float PosX, float PosY, float PosZ, float UpX, float UpY, float UpZ, float Yaw, float Pitch)
	: VFront(glm::vec3(0.0f, 0.0f, -1.0f)), FMovementSpeed(Speed), FMouseSensitivity(Sensitivity), FZoom(Zoom)
{
	VPosition = glm::vec3(PosX, PosY, PosZ);
	VWorldUp = glm::vec3(UpX, UpY, UpZ);
	FYaw = Yaw;
	FPitch = Pitch;
	updateCameraVectors();
}

Camera::Camera(glm::vec3 Pos, glm::vec3 Up, float Yaw, float Pitch)
	: VFront(glm::vec3(0.0f, 0.0f, 0.0f)), FMovementSpeed(Speed), FMouseSensitivity(Sensitivity), FZoom(Zoom)
{
	VPosition = Pos;
	VWorldUp = Up;
	FYaw = Yaw;
	FPitch = Pitch;
	updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const
{
	return lookAt(VPosition, VPosition + VFront, VUp);
}

glm::mat4 Camera::getProjectionMatrix(const float Width, const float Height) const
{
	return glm::perspective(glm::radians(FZoom), Width / Height, 0.1f, 100.0f);
}

void Camera::processKeyboard(const CameraMovement Direction, const float DeltaTime)
{
	float velocity = FMovementSpeed * DeltaTime;
	if (Direction == Forward)
		VPosition += VFront * velocity;
	if (Direction == Backward)
		VPosition -= VFront * velocity;
	if (Direction == Left)
		VPosition -= VRight * velocity;
	if (Direction == Right)
		VPosition += VRight * velocity;
	if (Direction == Up)
		VPosition += VWorldUp * velocity;
	if (Direction == Down)
		VPosition -= VWorldUp * velocity;
}

void Camera::processMouseMovement(float OffsetX, float OffsetY, const GLboolean ConstrainPitch)
{
	OffsetX *= FMouseSensitivity;
	OffsetY *= FMouseSensitivity;

	FYaw += OffsetX;
	FPitch += OffsetY;

	if (ConstrainPitch)
	{
		if (FPitch > 89.0f)
			FPitch = 89.0f;
		if (FPitch < -89.0f)
			FPitch = -89.0f;
	}

	updateCameraVectors();
}

void Camera::processMouseScroll(const float OffsetY)
{
	FZoom -= OffsetY;
	if (FZoom < 1.0f)
		FZoom = 1.0f;
	if (FZoom > 90.0f)
		FZoom = 90.0f;
}

void Camera::updateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(FYaw)) * cos(glm::radians(FPitch));
	front.y = sin(glm::radians(FPitch));
	front.z = sin(glm::radians(FYaw)) * cos(glm::radians(FPitch));
	VFront = normalize(front);
	VRight = normalize(cross(VFront, VWorldUp));
	VUp = normalize(cross(VRight, VFront));
}
