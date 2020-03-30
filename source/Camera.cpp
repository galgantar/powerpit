#include "Camera.h"

#include <glm.hpp>

void Camera::CalculateCameraVectors()
{
	right = glm::normalize(glm::cross(worldUp, front));
	up = glm::normalize(glm::cross(front, right));
}

Camera::Camera(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& worldUp, const float speed)
	:
	worldUp(worldUp),
	position(position),
	front(direction),
	up(worldUp),
	movementSpeed(speed)

{
	CalculateCameraVectors();
}

void Camera::MoveFront(const float deltaTime)
{
	position.z -= movementSpeed * deltaTime;
}

void Camera::MoveBack(const float deltaTime)
{
	position.z += movementSpeed * deltaTime;
}

void Camera::MoveLeft(const float deltaTime)
{
	position.x -= movementSpeed * deltaTime;
}
void Camera::MoveRight(const float deltaTime)
{
	position.x += movementSpeed * deltaTime;
}

void Camera::MoveUp(const float deltaTime)
{
	position.y += movementSpeed * deltaTime;
}
void Camera::MoveDown(const float deltaTime)
{
	position.y -= movementSpeed * deltaTime;
}

void Camera::MoveOnPos(const glm::vec3& newPosition)
{
	position = newPosition;
}

glm::mat4 Camera::GetViewMatrix()
{
	CalculateCameraVectors();
	
	return glm::lookAt(position, position + front, up);
}
