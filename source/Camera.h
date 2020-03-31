#pragma once
#include <vec3.hpp>
#include <gtc/matrix_transform.hpp>

class Camera {
private:
	const glm::vec3 worldUp;
	
	glm::vec3 position;
	glm::vec3 up;
	glm::vec3 front;
	glm::vec3 right;

	float yaw; // around y
	float pitch; // around x
	// float roll; // around z (currently not used)

	float movementSpeed;
	float sensitivity;

	void CalculateCameraVectors();

public:
	Camera(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& worldUp, const float speed=1.f, const float sensitivity=20.f);
	~Camera() {};

	void MoveFront(const float deltaTime);
	void MoveBack(const float deltaTime);
	void MoveLeft(const float deltaTime);
	void MoveRight(const float deltaTime);
	void MoveUp(const float deltaTime);
	void MoveDown(const float deltaTime);
	
	void MoveOnPos(const glm::vec3& newPosition);

	void Pitch(const double offset, float deltaTime);
	void Yaw(const double offset, float deltaTime);

	glm::mat4 GetViewMatrix();

};