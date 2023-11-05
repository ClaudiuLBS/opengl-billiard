#pragma once
class Physics {
public:
	static float frictionCoefficient;
	static float gravitationalAcceleration;

	static float NormalForceValue(float mass);

	static float FrictionForceValue(float mass);
};
