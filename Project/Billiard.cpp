#include "Billiard.h"

Billiard::Billiard(Ball whiteBall, std::vector<Ball> balls) {
	balls.insert(balls.begin(), whiteBall);
	this->balls = balls;

	initialPositions = new float[4 * (balls.size())];

	for (int i = 0; i < balls.size(); i++)
		for (int j = 0; j < 4; j++)
			initialPositions[i * 4 + j] = balls[i].initialPosition[j];
}

Ball& Billiard::WhiteBall() {
	return balls[0];
}

int Billiard::GetBallsBytesSize() {
	return balls.size() * 4 * sizeof(float);
}

void Billiard::BallsCollision(int ball_1_idx, int ball_2_idx) {
	Ball& ball_1 = balls[ball_1_idx];
	Ball& ball_2 = balls[ball_2_idx];

	float tempVar = 0.05f;

	float xDistance = abs(ball_1.currentPosition[0] - ball_2.currentPosition[0]);
	float yDistance = abs(ball_1.currentPosition[1] - ball_2.currentPosition[1]);

	//if (xDistance < tempVar / 4 || yDistance < tempVar / 2) return; // too close
	if (xDistance > tempVar / 2 || yDistance > tempVar) return; // too far


	glm::vec4 normalVector = ball_2.currentPosition - ball_1.currentPosition;
	normalVector = glm::normalize(normalVector);

	glm::vec4 tangentVector = { -normalVector[1], normalVector[0], 0, 0 };

	glm::vec4 v1 = ball_1.velocity;
	glm::vec4 v2 = ball_2.velocity;

	float v1n = glm::dot(normalVector, v1);
	float v1t = glm::dot(tangentVector, v1);

	float v2n = glm::dot(normalVector, v2);
	float v2t = glm::dot(tangentVector, v2);

	ball_1.velocity -= v2n * normalVector + v2n * tangentVector;
	ball_2.velocity += v1n * normalVector + v1n * tangentVector;

	float velocityReducer = 0.7f;
	ball_1.velocity *= velocityReducer;
	ball_2.velocity *= velocityReducer;
}

Billiard::~Billiard() {
	delete[] initialPositions;
}
