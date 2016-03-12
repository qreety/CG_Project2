#version 330 core

layout (location = 0) in vec4 position;

<<<<<<< HEAD
//smooth out vec3 vNormal;  //in camera space
//smooth out vec3 vPos; // in world space
//out vec3 aColor;
=======
out vec3 vNormal;  //in camera space
flat out int m;
out vec3 vPos; // in world space
out vec4 aColor;
>>>>>>> parent of f5f9fa1... Ver 1.3

//uniform vec3 aLight;
//uniform mat4 MVP;
//uniform mat4 V;
//uniform mat4 M;

void main()
{
    // Output position of the vertex, in clip space : MVP * position
	gl_Position = position;

	// Position of the vertex, in worldspace : M * position
	//vPos = (M * vec4(position, 1)).xyz;
	
	// Normal of the the vertex, in camera space
	//vNormal = mat3(transpose(inverse(M))) * normal;

	//aColor = aLight;
}