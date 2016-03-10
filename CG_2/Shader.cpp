#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <GL/freeglut.h>
#include <glm.hpp>
#include "vec3.hpp"
#include "Shader.hpp"
using namespace std;

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path){
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Step 1: Read the Shader Code from file
	std::string vertexShaderCode;
	std::string fragmentShaderCode;
	std::ifstream vShaderStream(vertex_file_path, std::ios::in);
	std::ifstream fShaderStream(fragment_file_path, std::ios::in);
	if (vShaderStream.is_open()){
		std::string Line = "";
		while (getline(vShaderStream, Line))
			vertexShaderCode += "\n" + Line;
		vShaderStream.close();
	}
	else{
		return 0;
	}

	if (fShaderStream.is_open()){
		std::string Line = "";
		while (getline(fShaderStream, Line))
			fragmentShaderCode += "\n" + Line;
		fShaderStream.close();
	}
	else{
		return 0;
	}

	//Step 2: Compile the Shaders
	GLint Result = GL_FALSE;
	int InfoLogLength;
	const char* vShaderPointer = vertexShaderCode.c_str();
	const char* fShaderPointer = fragmentShaderCode.c_str();

	//Compile vertex shader
	printf("Compiling shader : %s\n", vertex_file_path);
	glShaderSource(VertexShaderID, 1, &vShaderPointer, NULL);
	glCompileShader(VertexShaderID);
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}
else
	printf("Vertex shader compiled!");

	//Compile fragment shader
	printf("Compiling shader : %s\n", fragment_file_path);
	glShaderSource(FragmentShaderID, 1, &fShaderPointer, NULL);
	glCompileShader(FragmentShaderID);
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}
else
	printf("Fragment shader compiled!");

	//Link Shader Program
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0){
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	//Delete the shaders
	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}