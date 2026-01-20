#version 330 core

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 lightDir;


void main()
{
    vec3 N = normalize(Normal);
    vec3 L = normalize(-lightDir);

    float diff = max(dot(N, L), 0.0f);

    vec3 color = vec3(1.0f, 0.5f, 0.2f);
    FragColor = vec4(color, 1.0) * diff;
}
