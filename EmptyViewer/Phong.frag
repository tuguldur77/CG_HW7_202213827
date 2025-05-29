#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 LightPos;

uniform vec3 viewPos; // Camera position in world coordinates

// Material properties
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float shininess;
uniform float Ia; // Ambient light intensity

void main()
{
    // Ambient
    vec3 ambient = ka * Ia;

    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = kd * diff;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = normalize(reflect(-lightDir, norm));  
    // For Blinn-Phong, use the halfway vector
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
    vec3 specular = ks * spec;

    vec3 result = ambient + diffuse + specular;

    // Gamma correction (sRGB)
    FragColor = vec4(pow(result, vec3(1.0/2.2)), 1.0);
}