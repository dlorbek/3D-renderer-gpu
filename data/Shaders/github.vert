#version 330 core

layout (location = 0) in vec3 aPos;    // vertex position
layout (location = 1) in vec3 aNormal; // vertex normal

out vec3 FragPos;   // position in world space for lighting
out vec3 Normal;    // normal in world space

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0f));
    Normal = vec3(model * vec4(aNormal, 1.0f));; // transform normals correctly
    gl_Position = proj * view * vec4(FragPos, 1.0f);
}
