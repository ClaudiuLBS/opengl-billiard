#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Ball {
public:
	glm::vec4 initialPosition;
	glm::vec4 currentPosition;
	glm::mat4 transform;
	glm::vec4 velocity;
	float mass;
	float acceleration;

	Ball();
	Ball(float x, float y);

	void Move(float x, float y);
	void ApplyForce(glm::vec4 direction = { 0,0,0,0 }, float power = 0);
	void CheckForWalls();
};