#version 330 core

layout (location = 0) in vec4 position;

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