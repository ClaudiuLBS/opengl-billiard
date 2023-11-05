#pragma once

#include "Ball.h"
#include <vector>

class Billiard {
public:
	std::vector<Ball> balls;
	float* initialPositions;

	Billiard(Ball whiteBall, std::vector<Ball> balls);

	Ball& WhiteBall();
	int GetBallsBytesSize();
	void BallsCollision(int ball_1_idx, int ball_2_idx);

	~Billiard();
};