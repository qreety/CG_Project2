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

std::vector<glm::vec3> vertices;
std::vector<glm::vec3> normals;
std::vector<int> mindices;

typedef enum { MODEL_CUBE, MODEL_COW, MODEL_PHONE } Model;
typedef enum { CCW, CW } Orientation; // default front-facing is CCW
typedef enum { FLAT, SMOOTH } Shading;
typedef enum { X, Y, Z} Rotation;

typedef struct{
	glm::vec3 v0, v1, v2;
	glm::vec3 normal[3];
	unsigned char Color[3];
	glm::vec3 face_normal;
} triangle;

typedef struct{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	GLfloat shine;
} material;

typedef struct{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec4 position;
	GLboolean on;
} light;

triangle *Tris;
material *Mate;
light sLight, dLight;

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
}

void Initialize(int argc, char **argv){ // To initialize GLUT and window

	glutInit(&argc, argv);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitWindowSize(800, 600);
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
			TwAddVarRW(bar, "Direction", rotDir, &model, " group = Dynamic_Light");
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
	gluPerspective(45.0, aspect, 1.0, 3000.0);

	ProjectionMatrix = glm::perspective(45.0f, 4.0f/3.0f , 1.0f, 1000.0f);
		
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();

	if (ori == 0)
		glFrontFace(GL_CCW);
	else if (ori == 1)
		glFrontFace(GL_CW);

	switch (model)
	{
	case 0:
		gluLookAt(0, 0, 1, 0, 0, -2, 0, 1, 0);
		ViewMatrix = glm::lookAt(glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, -2.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));
		camPos = { 0.0f, 1.0f, 0.0f };
		break;
	case 1:
		gluLookAt((x_min + x_max) / 2, (y_min + y_max) / 2, 800 + (z_min + z_max) / 2, (x_min + x_max) / 2, (y_min + y_max) / 2, (z_min + z_max) / 2, 0, 1, 0);
		ViewMatrix = glm::lookAt(glm::vec3( (x_min + x_max) / 2, (y_min + y_max) / 2, 800 + (z_min + z_max) / 2 ),
			glm::vec3( (x_min + x_max) / 2, (y_min + y_max) / 2, (z_min + z_max) / 2 ),
			glm::vec3( 0.0f, 1.0f, 0.0f ));
		camPos = { (x_min + x_max) / 2, (y_min + y_max) / 2, 800 + (z_min + z_max) / 2 };
		break;
	case 2:
		gluLookAt((x_min + x_max) / 2, (y_min + y_max) / 2 - 30, 400 + (z_min + z_max) / 2, (x_min + x_max) / 2, (y_min + y_max) / 2, (z_min + z_max) / 2, 0, 1, 0);
		glTranslatef(80, 100, 0);
		ViewMatrix = glm::lookAt(glm::vec3( (x_min + x_max) / 2 + 80, (y_min + y_max) / 2 + 70, 400 + (z_min + z_max) / 2 ),
			glm::vec3( (x_min + x_max) / 2 + 80, (y_min + y_max) / 2 + 100, (z_min + z_max) / 2 ),
			glm::vec3( 0.0f, 1.0f, 0.0f ));
		camPos = { (x_min + x_max) / 2 + 80, (y_min + y_max) / 2 + 70, 400 + (z_min + z_max) / 2 };
		break;
	}
	
	MVPMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix;
	//glUseProgram(programID);
	SetLights(camPos,objPos);
	SetUniform(programID, camPos, ModelMatrix, ViewMatrix, MVPMatrix, sLight, dLight);
	//glDrawArrays(GL_TRIANGLES, 0, NumTris);
	
	if (display_type == 0)
		glBegin(GL_POINTS);
	else if (display_type == 1){
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBegin(GL_TRIANGLES);
	}
	else{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBegin(GL_TRIANGLES);
	}


	//assign color to each mesh
	glColor3f(1.0f, 0.0f, 0.0f);
	for (int i = 0; i < NumTris; i++)
	{
		glVertex3f(Tris[i].v0.x, Tris[i].v0.y, Tris[i].v0.z);
		glVertex3f(Tris[i].v1.x, Tris[i].v1.y, Tris[i].v1.z);
		glVertex3f(Tris[i].v2.x, Tris[i].v2.y, Tris[i].v2.z);
	}

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

}

void SetLights(glm::vec3 camPos, glm::vec3 objPos){
	while (dLightr){
		switch (rotate)
		{
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		default:
			break;
		}
	}
}

void SetUniform(int programID, glm::vec3 camPos, glm::mat4 ModelMatrix, glm::mat4 ViewMatrix, glm::mat4	MVPMatrix, light sLight, light dLight ){
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
	GLuint aLightID = glGetUniformLocation(programID, "aLight");

	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVPMatrix[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
	//Global ambient light
	glUniform3f(aLightID, global_r, global_g, global_b);
	//Directional light
	glUniform4f(glGetUniformLocation(programID, "sLight.position"), 0.0f, -1.f, -1.0f, 0.0f);
	glUniform3f(glGetUniformLocation(programID, "sLight.ambient"), sLight.ambient.r, sLight.ambient.g, sLight.ambient.b);
	glUniform3f(glGetUniformLocation(programID, "sLight.diffuse"), sLight.diffuse.r, sLight.diffuse.g, sLight.diffuse.b);
	glUniform3f(glGetUniformLocation(programID, "sLight.specular"), sLight.specular.r, sLight.specular.g, sLight.specular.b);
	glUniform1i(glGetUniformLocation(programID, "sLight.on"), sLight.on);
	//Ponit light
	glUniform4f(glGetUniformLocation(programID, "dLight.position"), dLight.position.x, dLight.position.y, dLight.position.z, dLight.position.w);
	glUniform3f(glGetUniformLocation(programID, "dLight.ambient"), dLight.ambient.r, dLight.ambient.g, dLight.ambient.b);
	glUniform3f(glGetUniformLocation(programID, "dLight.diffuse"), dLight.diffuse.r, dLight.diffuse.g, dLight.diffuse.b);
	glUniform3f(glGetUniformLocation(programID, "dLight.specular"), dLight.specular.r, dLight.specular.g, dLight.specular.b);
	glUniform1i(glGetUniformLocation(programID, "dLight.on"), dLight.on);
	//Materials
	for (int i = 0; i < material_count; i++){
		glUniform3f(glGetUniformLocation(programID, "mat[i].ambient"), Mate[i].ambient.r, Mate[i].ambient.g, Mate[i].ambient.b);
		glUniform3f(glGetUniformLocation(programID, "mat[i].diffuse"), Mate[i].diffuse.r, Mate[i].diffuse.g, Mate[i].diffuse.b);
		glUniform3f(glGetUniformLocation(programID, "mat[i].specular"), Mate[i].specular.r, Mate[i].specular.g, Mate[i].specular.b);
		glUniform1f(glGetUniformLocation(programID, "mat[i].shine"), Mate[i].shine);
	}
	//Camera position
	glUniform3f(glGetUniformLocation(programID, "camPos"), camPos.x, camPos.y, camPos.z);

}

void TW_CALL Reset(void *clientDate){
	switch (model)
	{
	case 0:
		dLight.position = { 0.0f, 1.0f, 0.0f, 1.0f };
		break;
	case 1:
		dLight.position = { (x_min + x_max) / 2, (y_min + y_max) / 2, 800 + (z_min + z_max) / 2, 1.0f };
		break;
	case 2:
		dLight.position = { (x_min + x_max) / 2 + 80, (y_min + y_max) / 2 + 70, 400 + (z_min + z_max) / 2, 1.0f };
		break;
	}
}