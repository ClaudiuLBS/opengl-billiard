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

#include "Billiard.h"

int BALL_RADIUS = 8;
int SCREEN_WIDTH = 1000;
int SCREEN_HEIGHT = 500;

GLuint
	VaoId,
	VboId,
	ProgramId,
	myMatrixLocation,
	myColorLocation;

glm::mat4 myMatrix;


// INUTIL
float lastTime = 0;
float deltaTime = 0;
// INUTIL


std::vector<Ball> balls = {
		Ball(180, 150),
		Ball(185, 145), Ball(185, 155),
		Ball(190, 140), Ball(190, 150), Ball(190, 160),
		//Ball(195, 135), Ball(195, 145), Ball(195, 155), Ball(195, 165),
		//Ball(200, 130), Ball(200, 140), Ball(200, 150), Ball(200, 160), Ball(200, 170)
};

Billiard billiard(Ball(60, 150), balls);


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
			billiard.BallsCollision(i, j);
		}

	// set white color
	glUniform4f(myColorLocation, 1, 1, 1, 1);

	//set transform
	billiard.WhiteBall().ApplyForce();
	myMatrix = billiard.WhiteBall().transform;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	//draw the white ball
	glDrawArrays(GL_POINTS, 0, 1);

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