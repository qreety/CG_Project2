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

<<<<<<< HEAD
#define BUFFER_OFFSET(offset) ((GLvoid*) (NULL + offset))
=======
GLuint NumTris, material_count;
GLfloat x_min = FLT_MIN, x_max = FLT_MIN, 
		y_min = FLT_MIN, y_max = FLT_MIN, 
		z_min = FLT_MIN, z_max = FLT_MIN; //To calculate the model position
GLfloat global_r, global_g, global_b;
GLint model = 2, ori, sm, rotate, display_type = 2;
TwBar *bar;
GLfloat width, height;
GLboolean dLightr;
GLuint programID;
GLuint vertexarray, vertexbuffer, normalbuffer, mbuffer;
>>>>>>> parent of f5f9fa1... Ver 1.3

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

<<<<<<< HEAD
	glFlush();
=======
void Initialize(int, char*[]);
void InitializeUI(void);
void ResizeFunction(int, int);
void RenderFunction(void);
void ExitFuntion(int);
void TW_CALL Reset(void *clientDate);
void LoadTriangle(void);
void CreateGeometry(void);
void SetLights(glm::vec3, glm::vec3);
void SetUniform(int, glm::vec3, glm::mat4, glm::mat4, glm::mat4, light, light);

//Return the minimum value of three values
GLfloat min3(GLfloat x, GLfloat y, GLfloat z){
	return x < y ? (x < z ? x : z) : (y < z ? y : z);
}

//Return the maximum value of three values
GLfloat max3(GLfloat x, GLfloat y, GLfloat z){
	return x > y ? (x > z ? x : z) : (y > z ? y : z);
}

//Read in and load the model
bool readin(char *FileName) {
	const int MAX_MATERIAL_COUNT = 255;
	glm::vec3 ambient[MAX_MATERIAL_COUNT], diffuse[MAX_MATERIAL_COUNT], specular[MAX_MATERIAL_COUNT];
	GLfloat x_temp, y_temp, z_temp;
	GLfloat shine[MAX_MATERIAL_COUNT];
	char ch;

	FILE* fp = fopen(FileName, "r");
	if (fp == NULL){
		printf(FileName, "doesn't exist");
		exit(1);
	}

	fscanf(fp, "%c", &ch);
	while (ch != '\n')
		fscanf(fp, "%c", &ch);

	fscanf(fp, "# triangles = %d\n", &NumTris);
	fscanf(fp, "Material count = %d\n", &material_count);

	Mate = new material[material_count];
	for (int i = 0; i < material_count; i++){
		fscanf(fp, "ambient color %f %f %f\n", &(Mate[i].ambient.x), &(Mate[i].ambient.y), &(Mate[i].ambient.z));
		fscanf(fp, "diffuse color %f %f %f\n", &(Mate[i].diffuse.x), &(Mate[i].diffuse.y), &(Mate[i].diffuse.z));
		fscanf(fp, "specular color %f %f %f\n", &(Mate[i].specular.x), &(Mate[i].specular.y), &(Mate[i].specular.z));
		fscanf(fp, "material shine %f\n", &(Mate[i].shine));
	}

	fscanf(fp, "%c", &ch);
	while (ch != '\n') // skip documentation line
		fscanf(fp, "%c", &ch);

	Tris = new triangle[NumTris];
	for (int i = 0; i<NumTris; i++) // read triangles
	{
		fscanf(fp, "v0 %f %f %f %f %f %f %d\n",
			&(Tris[i].v0.x), &(Tris[i].v0.y), &(Tris[i].v0.z),
			&(Tris[i].normal[0].x), &(Tris[i].normal[0].y), &(Tris[i].normal[0].z),
			&(Tris[i].Color[0]));
		fscanf(fp, "v1 %f %f %f %f %f %f %d\n",
			&(Tris[i].v1.x), &(Tris[i].v1.y), &(Tris[i].v1.z),
			&(Tris[i].normal[1].x), &(Tris[i].normal[1].y), &(Tris[i].normal[1].z),
			&(Tris[i].Color[1]));
		fscanf(fp, "v2 %f %f %f %f %f %f %d\n",
			&(Tris[i].v2.x), &(Tris[i].v2.y), &(Tris[i].v2.z),
			&(Tris[i].normal[2].x), &(Tris[i].normal[2].y), &(Tris[i].normal[2].z),
			&(Tris[i].Color[2]));
		fscanf(fp, "face normal %f %f %f\n", &(Tris[i].face_normal.x), &(Tris[i].face_normal.y),
			&(Tris[i].face_normal.z));

		//Get the minimum and maximum value to calculate the position
		x_temp = min3(Tris[i].v0.x, Tris[i].v1.x, Tris[i].v2.x);
		y_temp = min3(Tris[i].v0.y, Tris[i].v1.y, Tris[i].v2.y);
		z_temp = min3(Tris[i].v0.z, Tris[i].v1.z, Tris[i].v2.z);
		x_min = x_min < x_temp ? x_min : x_temp;
		y_min = y_min < y_temp ? y_min : y_temp;
		z_min = z_min < z_temp ? z_min : z_temp;

		x_temp = max3(Tris[i].v0.x, Tris[i].v1.x, Tris[i].v2.x);
		y_temp = max3(Tris[i].v0.y, Tris[i].v1.y, Tris[i].v2.y);
		z_temp = max3(Tris[i].v0.z, Tris[i].v1.z, Tris[i].v2.z);
		x_min = x_min > x_temp ? x_min : x_temp;
		y_min = y_min > y_temp ? y_min : y_temp;
		z_min = z_min > z_temp ? z_min : z_temp;

		vertices.push_back(Tris[i].v0);
		normals.push_back(Tris[i].normal[0]);
		mindices.push_back(Tris[i].Color[0]);
		vertices.push_back(Tris[i].v1);
		normals.push_back(Tris[i].normal[1]);
		mindices.push_back(Tris[i].Color[1]);
		vertices.push_back(Tris[i].v2);
		normals.push_back(Tris[i].normal[2]);
		mindices.push_back(Tris[i].Color[2]);
	}

	fclose(fp);
	return true;
}

int main(int argc, char **argv)
{
	Initialize(argc, argv);
	InitializeUI();

	glutReshapeFunc(ResizeFunction);
	glutDisplayFunc(RenderFunction);
	glutIdleFunc(RenderFunction);

	glewInit();
	programID = LoadShaders("shader.vert", "shader.frag");
		
	glutMainLoop();
	TwTerminate();
>>>>>>> parent of f5f9fa1... Ver 1.3
}

int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(800, 600);
<<<<<<< HEAD
	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Assignment II");
=======
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	glutCreateWindow("Project II");

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	switch (sm)
	{
	case 0:
		glShadeModel(GL_FLAT);
		break;
	case 1:
		glShadeModel(GL_SMOOTH);
		break;
	default:
		glShadeModel(GL_FLAT);
		break;
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glReadBuffer(GL_BACK);
	glDrawBuffer(GL_BACK);
	glEnable(GL_CULL_FACE);

	// Initialize AntTweakBar
	TwInit(TW_OPENGL, NULL);

	// Set GLUT event callbacks
	glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
	glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	glutKeyboardFunc((GLUTkeyboardfun)TwEventKeyboardGLUT);
	glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT);
	TwGLUTModifiersFunc(glutGetModifiers);
}

void InitializeUI(){
	bar = TwNewBar("TweakBar");
	TwDefine(" TweakBar size='200 400' color='96 216 224' "); // change default tweak bar size and color

	{
		TwEnumVal modelEV[3] = { { MODEL_CUBE, "Cube" }, { MODEL_COW, "Cow" }, { MODEL_PHONE, "Phone" } };
		TwType modelType = TwDefineEnum("modelType", modelEV, 3);
		TwAddVarRW(bar, "Model", modelType, &model, " keyIncr = '<' keyDecr = '>'");
	}

	{
		TwEnumVal oriEV[2] = { { CW, "CW" }, { CCW, "CCW" } };
		TwType oriType = TwDefineEnum("oriType", oriEV, 2);
		TwAddVarRW(bar, "Orientation", oriType, &ori, " keyIncr = '<' keyDecr = '>'");
	}

	{
		TwEnumVal smEV[2] = { { FLAT, "FLAT" }, { SMOOTH, "SMOOTH" } };
		TwType smType = TwDefineEnum("smType", smEV, 2);
		TwAddVarRW(bar, "Shade Mode", smType, &sm, " keyIncr = '<' keyDecr = '>'");
	}

	{
		TwAddVarRW(bar, "gRed", TW_TYPE_FLOAT, &global_r, "group = Global_Light"
			" min=0 max=1.0 step=0.1");
		TwAddVarRW(bar, "gGreen", TW_TYPE_FLOAT, &global_g, "group = Global_Light"
			" min=0 max=1.0 step=0.1");
		TwAddVarRW(bar, "gBlue", TW_TYPE_FLOAT, &global_b, "group = Global_Light"
			" min=0 max=1.0 step=0.1");
	}

	{
		TwAddVarRW(bar, "sSwitch", TW_TYPE_BOOL32, &sLight.on, "group = Static_Light");

		TwAddVarRW(bar, "saRed", TW_TYPE_FLOAT, &sLight.ambient.r, "group = sAmbient"
			" min=0 max=1.0 step=0.1");
		TwAddVarRW(bar, "saGreen", TW_TYPE_FLOAT, &sLight.ambient.g, "group = sAmbient"
			" min=0 max=1.0 step=0.1");
		TwAddVarRW(bar, "saBlue", TW_TYPE_FLOAT, &sLight.ambient.b, "group = sAmbient"
			" min=0 max=1.0 step=0.1");

		TwAddVarRW(bar, "sdRed", TW_TYPE_FLOAT, &sLight.diffuse.r, "group = sDiffuse"
			" min=0 max=1.0 step=0.1");
		TwAddVarRW(bar, "sdGreen", TW_TYPE_FLOAT, &sLight.diffuse.g, "group = sDiffuse"
			" min=0 max=1.0 step=0.1");
		TwAddVarRW(bar, "sdBlue", TW_TYPE_FLOAT, &sLight.diffuse.b, "group = sDiffuse"
			" min=0 max=1.0 step=0.1");

		TwAddVarRW(bar, "ssRed", TW_TYPE_FLOAT, &sLight.specular.r, "group = sSpecular"
			" min=0 max=1.0 step=0.1");
		TwAddVarRW(bar, "ssGreen", TW_TYPE_FLOAT, &sLight.specular.g, "group = sSpecular"
			" min=0 max=1.0 step=0.1");
		TwAddVarRW(bar, "ssBlue", TW_TYPE_FLOAT, &sLight.specular.b, "group = sSpecular"
			" min=0 max=1.0 step=0.1");

		TwDefine("TweakBar/sAmbient group = Static_Light");
		TwDefine("TweakBar/sDiffuse group = Static_Light");
		TwDefine("TweakBar/sSpecular group = Static_Light");
	}

	{
		TwAddVarRW(bar, "dSwitch", TW_TYPE_BOOL32, &dLight.on, "group = Dynamic_Light");

		TwAddVarRW(bar, "daRed", TW_TYPE_FLOAT, &dLight.ambient.r, "group = dAmbient"
			" min=0 max=1.0 step=0.1");
		TwAddVarRW(bar, "daGreen", TW_TYPE_FLOAT, &dLight.ambient.g, "group = dAmbient"
			" min=0 max=1.0 step=0.1");
		TwAddVarRW(bar, "daBlue", TW_TYPE_FLOAT, &dLight.ambient.b, "group = dAmbient"
			" min=0 max=1.0 step=0.1");

		TwAddVarRW(bar, "ddRed", TW_TYPE_FLOAT, &dLight.diffuse.r, "group = dDiffuse"
			" min=0 max=1.0 step=0.1");
		TwAddVarRW(bar, "ddGreen", TW_TYPE_FLOAT, &dLight.diffuse.g, "group = dDiffuse"
			" min=0 max=1.0 step=0.1");
		TwAddVarRW(bar, "ddBlue", TW_TYPE_FLOAT, &dLight.diffuse.b, "group = dDiffuse"
			" min=0 max=1.0 step=0.1");

		TwAddVarRW(bar, "dsRed", TW_TYPE_FLOAT, &dLight.specular.r, "group = dSpecular"
			" min=0 max=1.0 step=0.1");
		TwAddVarRW(bar, "dsGreen", TW_TYPE_FLOAT, &dLight.specular.g, "group = dSpecular"
			" min=0 max=1.0 step=0.1");
		TwAddVarRW(bar, "dsBlue", TW_TYPE_FLOAT, &dLight.specular.b, "group = dSpecular"
			" min=0 max=1.0 step=0.1");

		TwAddVarRW(bar, "Rotation", TW_TYPE_BOOL8, &dLightr, "group = Dynamic_Light");
		{
			TwEnumVal rotEV[3] = { { X, "X-Axis" }, { Y, "Y-Axis" }, { Z, "Z-Axis" } };
			TwType rotDir = TwDefineEnum("rotDir", rotEV, 3);
			TwAddVarRW(bar, "Direction", rotDir, &rotate, " group = Dynamic_Light");
		}
		TwAddButton(bar, "Reset", Reset, NULL, " group = Dynamic_Light");

		TwDefine("TweakBar/dAmbient group = Dynamic_Light");
		TwDefine("TweakBar/dDiffuse group = Dynamic_Light");
		TwDefine("TweakBar/dSpecular group = Dynamic_Light");
	}
}

void RenderFunction(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	LoadTriangle();
	CreateGeometry();

	glm::mat4 ProjectionMatrix;
	glm::mat4 ViewMatrix;
	glm::mat4 ModelMatrix = glm::mat4(1.0f);
	glm::mat4 MVPMatrix;
	
	glm::vec3 camPos;
	glm::vec3 objPos;

	GLfloat global_ambient[] = { global_r, global_g, global_b, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

	//Perspective
	GLfloat aspect = (GLfloat)width / (GLfloat)height;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(45.0, aspect, 1.0, 3000.0);
>>>>>>> parent of f5f9fa1... Ver 1.3

	glutDisplayFunc(RenderFunction);

	glewInit();

	programID = LoadShaders("shader.vert", "testShader.frag");
	glUseProgram(programID);

<<<<<<< HEAD
	init();
=======
	glEnd();
	glFlush();

	TwDraw();

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glPopMatrix();
	glutSwapBuffers();
}

void ResizeFunction(int Width, int Height){
	glViewport(0, 0, (GLsizei)Width, (GLsizei)Height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 4.0f/3.0f, 1.0f, 3000.0f);

	glMatrixMode(GL_MODELVIEW);
	glutSwapBuffers();

	TwWindowSize(Width, Height);
}

void ExitFunction(int value){

}

void LoadTriangle(){
	switch (model){
	case 0: readin("cube.in");		break;
	case 1: readin("cow_up.in");	break;
	case 2: readin("phone.in");		break;
	default:readin("cube.in");		break;
	}
}

void CreateGeometry(){
	glGenVertexArrays(1, &vertexarray);
	glBindVertexArray(vertexarray);

	glGenBuffers(1, &vertexbuffer);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
		);

	glGenBuffers(1, &normalbuffer);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
		);

	glGenBuffers(1, &mbuffer);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, mbuffer);
	glBufferData(GL_ARRAY_BUFFER, mindices.size() * sizeof(int), &mindices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(
		2,
		1,
		GL_UNSIGNED_INT,
		GL_FALSE,
		0,
		(void*)0
		);
	glBindVertexArray(0);
>>>>>>> parent of f5f9fa1... Ver 1.3

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

