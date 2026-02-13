#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;

out vec4 FragColor;



void main()
{

	vec3 lightPos = vec3(0.0f, 2.0f, 2.0f);
	vec3 objectColor = Color;

    // Ambient
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * objectColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = diff * objectColor;

    // Combine
    vec3 result = ambient + diffuse;
    FragColor = vec4(result, 1.0f);
}
