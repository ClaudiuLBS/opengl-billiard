// Codul sursa este adaptat dupa OpenGLBook.com

#include <windows.h>  // biblioteci care urmeaza sa fie incluse
#include <stdlib.h> // necesare pentru citirea shader-elor
#include <stdio.h>
#include <GL/glew.h> // glew apare inainte de freeglut
#include <GL/freeglut.h> // nu trebuie uitat freeglut.h

#include "loadShaders.h"
#include "glm/glm.hpp"		//	Bibloteci utilizate pentru transformari grafice;
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <vector>

#define BALL_RADIUS 8
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 500

GLuint
	VaoId,
	VboId,
	ColorBufferId,
	ProgramId,
	myMatrixLocation,
	myColorLocation;

glm::mat4
	myMatrix,
	resizeMatrix;

float xMin = 0.0f, xMax = 300.0f, yMin = 0.0f, yMax = 300.0f;		//	Variabile pentru proiectia ortogonala;

// INUTIL
float lastTime = 0;
float deltaTime = 0;
//INUTIL

struct Physics {
public:
	static float frictionCoefficient;
	static float gravitationalAcceleration;

	static float NormalForceValue(float mass) {
		return mass * gravitationalAcceleration;
	}

	static float FrictionForceValue(float mass) {
		return NormalForceValue(mass) * frictionCoefficient;
	}
};
float Physics::frictionCoefficient = 0.6f;
float Physics::gravitationalAcceleration = 9.80665f;

class Ball {
public:
	glm::vec4 initialPosition;
	glm::vec4 currentPosition;
	glm::mat4 transform;
	glm::vec4 velocity;
	float mass;
	float acceleration;

	Ball() {}

	Ball(GLfloat x, GLfloat y) {
		initialPosition[0] = x;
		initialPosition[1] = y;
		initialPosition[2] = 0;
		initialPosition[3] = 1;

		transform = glm::ortho(xMin, xMax, yMin, yMax);
		currentPosition = transform * initialPosition;
		velocity = { 0, 0, 0, 1 };
		mass = 0.16f;
	}

	void Move(GLfloat x, GLfloat y) {
		transform = glm::translate(transform, glm::vec3(x, y, 0));
		currentPosition = transform * initialPosition;
	}

	void ApplyForce(glm::vec4 direction = {0,0,0,0}, float power = 0) {
		glm::vec4 appliedForce = direction * power;
		glm::vec4 frictionForce = -1.0f * glm::normalize(velocity) * Physics::FrictionForceValue(mass);

		glm::vec4 resultForce = appliedForce + frictionForce;

		velocity += resultForce / 100.0f;

		CheckForWalls();
		Move(velocity[0], velocity[1]);
	}

	void CheckForWalls() {
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

	void CheckBallsCollision(std::vector<Ball> balls) {
		/*for (auto ball : balls) {
			if (ball.currentPosition[0] == currentPosition[0])
				continue;

			float distance = glm::distance(currentPosition, ball.currentPosition);
			if (distance <= 0.065f) {
				auto collisionNormal = glm::normalize(ball.currentPosition - currentPosition);
				auto impulse = (2.0f * velocity) / (2 / mass) * collisionNormal;
				velocity += impulse / mass;
				ball.velocity -= impulse / mass;
			}
		}*/
	}
};


class Billiard {
public:
	std::vector<Ball> balls;
	GLfloat* initialPositions;

	Billiard(Ball whiteBall, std::vector<Ball> balls) {
		balls.insert(balls.begin(), whiteBall);
		this->balls = balls;

		initialPositions = new GLfloat[4 * (balls.size())];
		
		for (int i = 0; i < balls.size(); i++)
			for (int j = 0; j < 4; j++)
				initialPositions[i * 4 + j] = balls[i].initialPosition[j];
	}

	Ball& WhiteBall() {
		return balls[0];
	}

	int GetBallsBytesSize() {
		return balls.size() * 4 * sizeof(GLfloat);
	}

	~Billiard() {
		delete[] initialPositions;
	}
};

std::vector<Ball> balls = {
		Ball(180, 150),
		Ball(185, 145), Ball(185, 155),
		Ball(190, 140), Ball(190, 150), Ball(190, 160),
		//Ball(195, 135), Ball(195, 145), Ball(195, 155), Ball(195, 165),
		//Ball(200, 130), Ball(200, 140), Ball(200, 150), Ball(200, 160), Ball(200, 170)
};

Billiard billiard(Ball(60, 150), balls);

void BallsCollision(Ball& ball_1, Ball& ball_2) {
	float tempVar = 0.05f;

	float xDistance = abs(ball_1.currentPosition[0] - ball_2.currentPosition[0]);
	float yDistance = abs(ball_1.currentPosition[1] - ball_2.currentPosition[1]);

	std::cout << '\r' << xDistance << ' ' << yDistance;
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

	//std::cout << ball_1.currentPosition[0] << ' ' << ball_1.currentPosition[1] << std::endl;
	//std::cout << v2n << ' ' << v1t << std::endl;
	ball_1.velocity -= v2n * normalVector + v2n * tangentVector;
	ball_2.velocity += v1n * normalVector + v1n * tangentVector;

	float velocityReducer = 0.7f;
	ball_1.velocity *= velocityReducer;
	ball_2.velocity *= velocityReducer;
}


void CreateVBO(void)
{
	// se creeaza un buffer nou
	glGenBuffers(1, &VboId);
	// este setat ca buffer curent
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	// varfurile sunt "copiate" in bufferul curent
	glBufferData(GL_ARRAY_BUFFER, billiard.GetBallsBytesSize(), billiard.initialPositions, GL_STATIC_DRAW);

	// se creeaza / se leaga un VAO (Vertex Array Object) - util cand se utilizeaza mai multe VBO
	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);

	// se activeaza lucrul cu atribute; atributul 0 = pozitie
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

}
void DestroyVBO(void)
{
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &ColorBufferId);
	glDeleteBuffers(1, &VboId);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);
}

void CreateShaders(void)
{
	ProgramId = LoadShaders("example.vert", "example.frag");
	glUseProgram(ProgramId);
}

void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}

void Initialize(void)
{
	glClearColor(0.08f, 0.35f, 0.26f, 1.0f);
	CreateVBO();
	CreateShaders();	

	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	myColorLocation = glGetUniformLocation(ProgramId, "myColor");
	myMatrix = resizeMatrix;
}


void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(BALL_RADIUS * 2);


	//std::cout << '\r' << billiard.balls.size();
	// Calculate collisions
	for (int i = 0; i < billiard.balls.size() - 1; i++)
		for (int j = i + 1; j < billiard.balls.size(); j++) {
			std::cout << '\r' << j;
			BallsCollision(billiard.balls[i], billiard.balls[j]);
		}

	// set white color
	glUniform4f(myColorLocation, 1, 1, 1, 1);

	//set transform
	billiard.WhiteBall().ApplyForce();
	myMatrix = billiard.WhiteBall().transform;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	//draw the white ball
	glDrawArrays(GL_POINTS, 0, 1);

	//std::cout << "\r" << glm::distance(billiard.whiteBall.currentPosition, billiard.balls[1].currentPosition);
	//std::cout << '\r' << billiard.whiteBall.currentPosition[0] << ' ' << billiard.whiteBall.currentPosition[1];
	// drawing the rest of the balls ([0] is the white ball)
	for (int i = 1; i < billiard.balls.size(); i++) {
		// set red color
		glUniform4f(myColorLocation, 1, 0.4f, 0, 1);
		billiard.balls[i].ApplyForce();

		//set transform
		myMatrix = billiard.balls[i].transform;
		glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

		//draw the ball
		glDrawArrays(GL_POINTS, i, 1);
	}



	glFlush();
}
void Cleanup(void)
{
	DestroyShaders();
	DestroyVBO();
}

void UpdateAnimation(int value) {
	glutPostRedisplay();
	
	//INUTIL
	float elapsedTime = glutGet(GLUT_ELAPSED_TIME) / 1000;
	deltaTime = elapsedTime - lastTime;
	lastTime = elapsedTime;
	//INUTIL

	glutTimerFunc(16, UpdateAnimation, 0);
}

float GetMouseX(int x) {

	return (2.0f * x / SCREEN_WIDTH) - 1.0f;
}
float GetMouseY(int y) {
	return 1.0f - (2.0f * y / SCREEN_HEIGHT);
}

void MouseClick(int button, int state, int x, int y) {
	int FORCE = 250;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		glm::vec4 ballPosition = billiard.WhiteBall().currentPosition;
		glm::vec4 direction = { GetMouseX(x) - ballPosition[0], GetMouseY(y) - ballPosition[1], 0, 0};
		//direction = glm::normalize(direction);
		billiard.WhiteBall().ApplyForce(direction, FORCE);
	}
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(100, 100); // pozitia initiala a ferestrei
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT); //dimensiunile ferestrei
	glutCreateWindow("Billiard"); // titlul ferestrei
	glewInit(); // nu uitati de initializare glew; trebuie initializat inainte de a a initializa desenarea
	Initialize();

	glutDisplayFunc(RenderFunction);

	glutTimerFunc(0, UpdateAnimation, 0); // functia asta randeaza din nou fereastra

	glutMouseFunc(MouseClick);

	glutCloseFunc(Cleanup);
	glutMainLoop();
}