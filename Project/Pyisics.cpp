#include "Physics.h"

float Physics::frictionCoefficient = 0.6f;
float Physics::gravitationalAcceleration = 9.80665f;

float Physics::NormalForceValue(float mass) {
	return mass * gravitationalAcceleration;
}

float Physics::FrictionForceValue(float mass) {
	return NormalForceValue(mass) * frictionCoefficient;
}