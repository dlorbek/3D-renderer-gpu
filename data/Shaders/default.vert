#version 330 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;

layout (location = 1) in vec3 aNormal;



// Outputs the normal for the Fragment Shader
out vec3 Normal;
out vec3 fragPos;



uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;


void main()
{
	fragPos = vec3(model * vec4(aPos, 1.0));
	gl_Position = proj * view * vec4(fragPos, 1.0f);

	Normal = mat3(model) * aNormal;
}