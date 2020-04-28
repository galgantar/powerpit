#include "PCH.h"

#include "Camera.h"


void Camera::CalculateCameraVectors()
{
	front.x = cos(gm::radians(yaw)) * cos(gm::radians(pitch));
	front.y = sin(gm::radians(pitch));
	front.z = sin(gm::radians(yaw)) * cos(gm::radians(pitch));

	front = gm::normalize(front);
	right = gm::normalize(gm::cross(worldUp, front));
	up = gm::normalize(gm::cross(front, right));
}

Camera::Camera(const gm::vec3& position, const gm::vec3& direction, const gm::vec3& worldUp, const float speed, const float sensitivity)
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

void Camera::MoveOnPos(const gm::vec3& newPosition)
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

gm::mat4 Camera::GetViewMatrix()
{
	CalculateCameraVectors();
	
	return gm::lookAt(position, position + front, up);
}
