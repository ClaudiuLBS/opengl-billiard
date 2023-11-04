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

GLuint
	VaoId,
	VboId,
	ColorBufferId,
	ProgramId,
	myMatrixLocation;

glm::mat4
	myMatrix,
	transform,
	resizeMatrix;

float xMin = 0.0f, xMax = 260.0f, yMin = 0.0f, yMax = 130.0f;		//	Variabile pentru proiectia ortogonala;

void CreateVBO(void)
{
	// varfurile 
	GLfloat Vertices[] = {
		60.0f, 65.0f, 0.0f, 1.0f,
		90.0f, 65.0f, 0.0f, 1.0f,
	};

	// culorile, ca atribute ale varfurilor
	GLfloat Colors[] = {
	  1.0f, 1.0f, 1.0f, 1.0f,
	  1.0f, 1.0f, 1.0f, 1.0f,
	};

	// se creeaza un buffer nou
	glGenBuffers(1, &VboId);
	// este setat ca buffer curent
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	// varfurile sunt "copiate" in bufferul curent
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	// se creeaza / se leaga un VAO (Vertex Array Object) - util cand se utilizeaza mai multe VBO
	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);
	// se activeaza lucrul cu atribute; atributul 0 = pozitie
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	// un nou buffer, pentru culoare
	glGenBuffers(1, &ColorBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
	// atributul 1 =  culoare
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

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
	resizeMatrix = glm::ortho(xMin, xMax, yMin, yMax);
	myMatrix = resizeMatrix;
}

void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	myMatrix = glm::translate(myMatrix, glm::vec3(0, 0, 0));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glEnable(GL_POINT_SMOOTH);

	glPointSize(15);
	glDrawArrays(GL_POINTS, 0, 2);
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

