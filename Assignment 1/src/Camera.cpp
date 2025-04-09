/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2025 Media Design School

File Name : Camera.cpp
Description : Implementations for Camera class
Author : Shikomisen (Ayoub Ahmad)
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#include "Camera.h"

Camera::Camera(float PosX, float PosY, float PosZ, float UpX, float UpY, float UpZ, float InputYaw, float InputPitch)
	: PbFront(glm::vec3(0.0f, 0.0f, -1.0f)), PbMovementSpeed(Speed), PbMouseSensitivity(Sensitivity), PbZoom(Zoom)
{
	PbPosition = glm::vec3(PosX, PosY, PosZ);
	PbWorldUp = glm::vec3(UpX, UpY, UpZ);
	PbYaw = InputYaw;
	PbPitch = InputPitch;
	updateCameraVectors();
}

Camera::Camera(glm::vec3 Pos, glm::vec3 Up, float InputYaw, float InputPitch)
	: PbFront(glm::vec3(0.0f, 0.0f, 0.0f)), PbMovementSpeed(Speed), PbMouseSensitivity(Sensitivity), PbZoom(Zoom)
{
	PbPosition = Pos;
	PbWorldUp = Up;
	PbYaw = InputYaw;
	PbPitch = InputPitch;
	updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const
{
	return lookAt(PbPosition, PbPosition + PbFront, PbUp);
}

glm::mat4 Camera::getProjectionMatrix(const float Width, const float Height) const
{
	return glm::perspective(glm::radians(PbZoom), Width / Height, 0.1f, 100.0f);
}

void Camera::processKeyboard(const CameraMovement Direction, const float DeltaTime)
{
	const float Velocity = PbMovementSpeed * DeltaTime;
	if (Direction == Forward)
		PbPosition += PbFront * Velocity;
	if (Direction == Backward)
		PbPosition -= PbFront * Velocity;
	if (Direction == Left)
		PbPosition -= PbRight * Velocity;
	if (Direction == Right)
		PbPosition += PbRight * Velocity;
	if (Direction == Up)
		PbPosition += PbWorldUp * Velocity;
	if (Direction == Down)
		PbPosition -= PbWorldUp * Velocity;
}

void Camera::processMouseMovement(float OffsetX, float OffsetY, const GLboolean ConstrainPitch)
{
	OffsetX *= PbMouseSensitivity;
	OffsetY *= PbMouseSensitivity;

	PbYaw += OffsetX;
	PbPitch += OffsetY;

	if (ConstrainPitch)
	{
		if (PbPitch > 89.0f)
			PbPitch = 89.0f;
		if (PbPitch < -89.0f)
			PbPitch = -89.0f;
	}

	updateCameraVectors();
}

void Camera::processMouseScroll(const float OffsetY)
{
	PbZoom -= OffsetY;
	if (PbZoom < 1.0f)
		PbZoom = 1.0f;
	if (PbZoom > 90.0f)
		PbZoom = 90.0f;
}

void Camera::reset()
{
	updateCameraVectors();
}

void Camera::updateCameraVectors()
{
	glm::vec3 Front;
	Front.x = cos(glm::radians(PbYaw)) * cos(glm::radians(PbPitch));
	Front.y = sin(glm::radians(PbPitch));
	Front.z = sin(glm::radians(PbYaw)) * cos(glm::radians(PbPitch));
	PbFront = normalize(Front);
	PbRight = normalize(cross(PbFront, PbWorldUp));
	PbUp = normalize(cross(PbRight, PbFront));
}
