#include "Camera.h"

#include <glm.hpp>


void Camera::CalculateCameraVectors()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	front = glm::normalize(front);
	right = glm::normalize(glm::cross(worldUp, front));
	up = glm::normalize(glm::cross(front, right));
}

Camera::Camera(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& worldUp, const float speed, const float sensitivity)
	:
	worldUp(worldUp),
	position(position),
	movementSpeed(speed),
	sensitivity(sensitivity),
	yaw(-90.f),
	pitch(0.f)
{}

void Camera::MoveFront(const float deltaTime)
{
	CalculateCameraVectors();
	position += front * movementSpeed * deltaTime;
}

void Camera::MoveBack(const float deltaTime)
{
	CalculateCameraVectors();
	position -= front * movementSpeed * deltaTime;
}

void Camera::MoveLeft(const float deltaTime)
{
	CalculateCameraVectors();
	position += right * movementSpeed * deltaTime;
}
void Camera::MoveRight(const float deltaTime)
{
	CalculateCameraVectors();
	position -= right * movementSpeed * deltaTime;
}

void Camera::MoveUp(const float deltaTime)
{
	CalculateCameraVectors();
	position += up * movementSpeed * deltaTime;
}
void Camera::MoveDown(const float deltaTime)
{
	CalculateCameraVectors();
	position -= up * movementSpeed * deltaTime;
}

void Camera::MoveOnPos(const glm::vec3& newPosition)
{
	position = newPosition;
}

void Camera::Pitch(const double offset, float deltaTime)
{
	pitch += offset * sensitivity * deltaTime;

	if (pitch > 90.f)
		pitch = 90.f;
	
	else if (pitch < -90.f)
		pitch = -90.f;

	/*
	uncomment in case of removed borders for pitch

	if (pitch > 360.f)
		pitch -= 360.f;
	*/
}

void Camera::Yaw(const double offset, float deltaTime)
{
	yaw += offset * sensitivity * deltaTime;

	if (yaw > 360.f)
		yaw -= 360.f;
}

glm::mat4 Camera::GetViewMatrix()
{
	CalculateCameraVectors();
	
	return glm::lookAt(position, position + front, up);
}
