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

float xMin = 0.0f, xMax = 260.0f, yMin = 0.0f, yMax = 130.0f;		//	Variabile pentru proiectia ortogonala;


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

		currentPosition[0] = x;
		currentPosition[1] = y;
		currentPosition[2] = 0;
		currentPosition[3] = 1;

		transform = glm::ortho(xMin, xMax, yMin, yMax);
	}

	void Move(GLfloat x, GLfloat y) {
		transform = glm::translate(transform, glm::vec3(x, y, 0));
		currentPosition = initialPosition * transform;
	}
};

class Billiard {
public:
	Ball whiteBall;
	std::vector<Ball> balls;
	GLfloat* initialPositions;

	Billiard(Ball whiteBall, std::vector<Ball> balls) {
		this->whiteBall = whiteBall;
		balls.insert(balls.begin(), whiteBall);
		this->balls = balls;

		initialPositions = new GLfloat[4 * (balls.size())];
		
		for (int i = 0; i < balls.size(); i++)
			for (int j = 0; j < 4; j++)
				initialPositions[i * 4 + j] = balls[i].initialPosition[j];
	}

	int GetBallsBytesSize() {
		return balls.size() * 4 * sizeof(GLfloat);
	}

	~Billiard() {
		delete[] initialPositions;
	}
};


std::vector<Ball> balls = {
		Ball(180, 65),
		Ball(185, 62), Ball(185, 68),
		Ball(190, 60), Ball(190, 65), Ball(190, 70)
};

Billiard billiard(Ball(60, 65), balls);


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
	glPointSize(15);

	// set white color
	glUniform4f(myColorLocation, 1, 1, 1, 1);

	//set transform
	myMatrix = billiard.whiteBall.transform;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	//draw the white ball
	glDrawArrays(GL_POINTS, 0, 1);

	// drawing the rest of the balls ([0] is the white ball)
	for (int i = 1; i < billiard.balls.size(); i++) {
		// set red color
		glUniform4f(myColorLocation, 1, 0, 0, 1);

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
	glutTimerFunc(16, UpdateAnimation, 0);
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(100, 100); // pozitia initiala a ferestrei
	glutInitWindowSize(1000, 500); //dimensiunile ferestrei
	glutCreateWindow("Billiard"); // titlul ferestrei
	glewInit(); // nu uitati de initializare glew; trebuie initializat inainte de a a initializa desenarea
	Initialize();
	glutDisplayFunc(RenderFunction);
	glutTimerFunc(0, UpdateAnimation, 0);
	glutCloseFunc(Cleanup);
	glutMainLoop();
}

