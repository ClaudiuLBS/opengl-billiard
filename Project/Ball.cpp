#include "Ball.h"
#include "Physics.h"

Ball::Ball() {}

Ball::Ball(float x, float y) {
	initialPosition[0] = x;
	initialPosition[1] = y;
	initialPosition[2] = 0;
	initialPosition[3] = 1;

	transform = glm::ortho(0.0f, 300.0f, 0.0f, 300.0f);
	currentPosition = transform * initialPosition;
	velocity = { 0, 0, 0, 1 };
	mass = 0.16f;
}

void Ball::Move(float x, float y) {
	transform = glm::translate(transform, glm::vec3(x, y, 0));
	currentPosition = transform * initialPosition;
}

void Ball::ApplyForce(glm::vec4 direction, float power) {
	glm::vec4 appliedForce = direction * power;
	glm::vec4 frictionForce = -1.0f * glm::normalize(velocity) * Physics::FrictionForceValue(mass);

	glm::vec4 resultForce = appliedForce + frictionForce;

	velocity += resultForce / 100.0f;

	CheckForWalls();
	Move(velocity[0], velocity[1]);
}

void Ball::CheckForWalls() {
	glm::vec4 wallsNormals[4] = {
		{  1,  0, 0, 0 }, // stanga
		{  0, -1, 0, 0 }, // sus
		{ -1,  0, 0, 0 }, // dreapta
		{  0,  1, 0, 0 }, // jos
	};

	float speedReduction = 1;

	if (currentPosition[0] < -0.98f) // stanga
		velocity = speedReduction * (velocity - 2.0f * glm::dot(velocity, wallsNormals[0]) * wallsNormals[0]);
	else if (currentPosition[1] > 0.98f) // sus
		velocity = speedReduction * (velocity - 2.0f * glm::dot(velocity, wallsNormals[1]) * wallsNormals[1]);
	else if (currentPosition[0] > 0.98f) // dreapta
		velocity = speedReduction * (velocity - 2.0f * glm::dot(velocity, wallsNormals[2]) * wallsNormals[2]);
	else if (currentPosition[1] < -0.98f) // jos
		velocity = speedReduction * (velocity - 2.0f * glm::dot(velocity, wallsNormals[3]) * wallsNormals[3]);
}
