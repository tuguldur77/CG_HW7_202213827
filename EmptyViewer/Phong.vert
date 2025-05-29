#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos; // Light position in world coordinates

out vec3 FragPos;
out vec3 Normal;
out vec3 LightPos; // Pass light position to fragment shader

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    
    // Pass light position (transformed to world space if needed, here assuming world space)
    LightPos = lightPos; 

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}