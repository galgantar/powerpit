#pragma once

#include "PCH.h"


class Camera
{
private:
	const gm::vec3 worldUp;
	
	gm::vec3 position;
	gm::vec3 up;
	gm::vec3 front;
	gm::vec3 right;

	float yaw; // around y
	float pitch; // around x
	// float roll; // around z (currently not used)

	float movementSpeed;
	float sensitivity;

	void CalculateCameraVectors();

public:
	Camera(const gm::vec3& position, const gm::vec3& worldUp, const float speed=1.f, const float sensitivity=20.f);
	~Camera() {};

	inline gm::vec3 GetPosition() { return position; }
	inline gm::vec3 GetFront() { return front; }

	void MoveFront(const float deltaTime);
	void MoveBack(const float deltaTime);
	void MoveLeft(const float deltaTime);
	void MoveRight(const float deltaTime);
	void MoveUp(const float deltaTime);
	void MoveDown(const float deltaTime);
	
	void MoveOnPos(const gm::vec3& newPosition);

	void Pitch(const double offset, float deltaTime);
	void Yaw(const double offset, float deltaTime);

	gm::mat4 GetViewMatrix();

};