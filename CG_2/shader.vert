#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in int mindex;

smooth out vec3 vNormal;  //in camera space
flat out int m;
smooth out vec3 vPos; // in world space
out vec4 aColor;

uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;

void main()
{
    // Output position of the vertex, in clip space : MVP * position
	gl_Position = MVP * vec4(position, 1);

	// Position of the vertex, in worldspace : M * position
	vPos = (M * vec4(position, 1)).xyz;
	
	// Normal of the the vertex, in camera space
	vNormal = mat3(transpose(inverse(M))) * normal;

	m = mindex;
}