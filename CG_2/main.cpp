/*******************************************************
*	Reference: openglbook.com
*	opengl-tutorial.org
*	The red book
*	anttweakbar.sourceforge.net
*/
#define _CRT_SECURE_NO_DEPRECATE

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <glm.hpp>
#include "gtc/matrix_transform.hpp"
#include "Shader.hpp"
#include "vec3.hpp"
#include <vector>
#include <AntTweakBar.h>

#define BUFFER_OFFSET(offset) ((GLvoid*) (NULL + offset))

enum VAO_IDs{Triangles, NumVAOs};
enum Buffer_IDs{ArrayBuffer, NumBuffers};
enum Attrib_IDs{vPosition = 0};

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
const GLuint NumVert = 6;
GLuint programID;

void init(void);

void RenderFunction(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glBindVertexArray(VAOs[Triangles]);

	glDrawArrays(GL_TRIANGLES, 0, NumVert);

	glFlush();
}

int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(800, 600);
	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Assignment II");

	glutDisplayFunc(RenderFunction);

	glewInit();

	programID = LoadShaders("shader.vert", "testShader.frag");
	glUseProgram(programID);

	init();

	glutMainLoop();
}

void init(void){
		
	if (glGenVertexArrays == NULL)
	{
		printf("GLEW not initialized");
	}

	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[0]);

	GLfloat vertices[NumVert][2] = {
		{ -0.9f, -0.9f  },
		{0.85f, -0.9f },
		{-0.9f, 0.85f},
		{0.9f, -0.85f},
		{0.9f, 0.9f},
		{-0.85f, 0.9f}
	};

	glGenBuffers(1, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(vPosition);
}

