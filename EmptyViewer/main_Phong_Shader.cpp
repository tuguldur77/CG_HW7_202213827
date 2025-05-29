#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#define NOMINMAX
#include <Windows.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/freeglut.h>

#define GLFW_INCLUDE_GLU
#define GLFW_DLL
#include <GLFW/glfw3.h>

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp> 

#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>

using namespace glm;

int Width = 512;
int Height = 512;

// Sphere data
std::vector<glm::vec3> gPositions;
std::vector<glm::vec3> gNormals;
std::vector<unsigned int> gIndices;

void resize_callback(GLFWwindow* window, int nw, int nh) {
    Width = nw;
    Height = nh;
    glViewport(0, 0, nw, nh);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

// Function to load and compile shaders (reused from HW7_sample_code.cpp)
GLuint LoadShaders(const std::string& vertex_file_path, const std::string& fragment_file_path) {
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path.c_str(), std::ios::in);
    if (VertexShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << VertexShaderStream.rdbuf();
        VertexShaderCode = sstr.str();
        VertexShaderStream.close();
    }
    else {
        printf("Impossible to open %s. Are you in the right directory? Don't forget to read the FAQ !\n", vertex_file_path.c_str());
        getchar();
        return 0;
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path.c_str(), std::ios::in);
    if (FragmentShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << FragmentShaderStream.rdbuf();
        FragmentShaderCode = sstr.str();
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    printf("Compiling shader: %s\n", vertex_file_path.c_str());
    char const* VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }

    // Compile Fragment Shader
    printf("Compiling shader: %s\n", fragment_file_path.c_str());
    char const* FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }

    // Link the program
    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

// Function to update mesh data in VBOs (modified from HW7_sample_code.cpp to remove Colors)
void Update_mesh(const GLuint& VAO, const std::vector<GLuint>& GLBuffers,
    std::vector<glm::vec3>& Positions,
    std::vector<glm::vec3>& Normals,
    std::vector<unsigned int>& Indices) {
    glBindVertexArray(VAO); // must be bound before binding buffers

    // Bind and update positions
    glBindBuffer(GL_ARRAY_BUFFER, GLBuffers[0]);
    glBufferData(GL_ARRAY_BUFFER, Positions.size() * sizeof(Positions[0]), &Positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Bind and update normals
    glBindBuffer(GL_ARRAY_BUFFER, GLBuffers[1]);
    glBufferData(GL_ARRAY_BUFFER, Normals.size() * sizeof(Normals[0]), &Normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Bind and update indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GLBuffers[2]); // GLBuffers[2] is now for indices
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(Indices[0]), &Indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0); // Unbind VAO to avoid accidental modification
}

// Reuse from HW5: sphere generation
void generateSphere(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals, std::vector<unsigned int>& indices) {
    int width = 32;
    int height = 16;
    int numVertices = (height - 2) * width + 2;
    int numTriangles = (height - 2) * (width - 1) * 2;
    positions.resize(numVertices);
    normals.resize(numVertices);
    indices.reserve(numTriangles * 3); // Reserve space for indices

    int t = 0;
    for (int j = 1; j < height - 1; ++j) {
        for (int i = 0; i < width; ++i) {
            float theta = float(j) / (height - 1) * M_PI;
            float phi = float(i) / (width - 1) * M_PI * 2;
            float x = sinf(theta) * cosf(phi);
            float y = cosf(theta);
            float z = -sinf(theta) * sinf(phi);
            positions[t] = glm::vec3(x, y, z);
            normals[t] = glm::normalize(positions[t]);
            t++;
        }
    }
    positions[t] = glm::vec3(0.0f, 1.0f, 0.0f); normals[t++] = glm::vec3(0.0f, 1.0f, 0.0f);
    positions[t] = glm::vec3(0.0f, -1.0f, 0.0f); normals[t++] = glm::vec3(0.0f, -1.0f, 0.0f);

    for (int j = 0; j < height - 3; ++j) {
        for (int i = 0; i < width - 1; ++i) {
            indices.push_back(j * width + i);
            indices.push_back((j + 1) * width + i + 1);
            indices.push_back(j * width + i + 1);
            indices.push_back(j * width + i);
            indices.push_back((j + 1) * width + i);
            indices.push_back((j + 1) * width + i + 1);
        }
    }
    for (int i = 0; i < width - 1; ++i) {
        indices.push_back((height - 2) * width); indices.push_back(i); indices.push_back(i + 1);
        indices.push_back((height - 2) * width + 1); indices.push_back((height - 3) * width + i + 1); indices.push_back((height - 3) * width + i);
    }
}


int main(void) {
    GLFWwindow* window;
    // Initialise GLFW
    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(Width, Height, "OpenGL Phong Shaded Sphere", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, resize_callback);

    // Initialize GLEW
    glewExperimental = true; 
    if (glewInit() != GLEW_OK) {
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //glEnable(GL_CULL_FACE); // Enable back-face culling for better performance, as suggested in HW6 example

    // Create and compile our GLSL program from the shaders
    GLuint shaderProgram = LoadShaders("Phong.vert", "Phong.frag");

    // Generate sphere data
    generateSphere(gPositions, gNormals, gIndices);

    // Generate VAO and VBOs
    GLuint VAO;
    glGenVertexArrays(1, &VAO);

    const int numBuffers = 3; // Positions, Normals, Indices
    std::vector<GLuint> GLBuffers;
    GLBuffers.resize(numBuffers, 0);
    glGenBuffers(numBuffers, &GLBuffers[0]);

    // Update the mesh data in the VBOs
    Update_mesh(VAO, GLBuffers, gPositions, gNormals, gIndices);

    // Get uniform locations
    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
    GLuint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos"); // Added viewPos uniform location

    GLuint kaLoc = glGetUniformLocation(shaderProgram, "ka");
    GLuint kdLoc = glGetUniformLocation(shaderProgram, "kd");
    GLuint ksLoc = glGetUniformLocation(shaderProgram, "ks");
    GLuint shininessLoc = glGetUniformLocation(shaderProgram, "shininess");
    GLuint IaLoc = glGetUniformLocation(shaderProgram, "Ia"); // Consistent with fragment shader

    // Define transformation matrices
    glm::mat4 model = glm::mat4(1.0f);
    // Transform the unit sphere to a sphere centered at (0, 0, -7) with a radius of 2.
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -7.0f));
    model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));

    glm::mat4 view = glm::mat4(1.0f); // Camera at (0, 0, 0), looking along -w (0, 0, 1) with (0,1,0) as up
    // This is equivalent to glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 projection = glm::mat4(1.0f);
    float l = -0.1f, r = 0.1f, b = -0.1f, t = 0.1f, n = -0.1f, f = -1000.0f; // Note: n and f are negative in problem description
    // For glm::frustum, near and far must be positive distances.
    // The perspective projection transform with l = -0.1, r = 0.1, b = -0.1, t= 0.1, n = -0.1, f = -1000.
    // In glm::frustum, the parameters are (left, right, bottom, top, nearVal, farVal)
    // where nearVal and farVal are positive distances from the viewer.
    // So, if n is -0.1 (meaning -0.1 units along -Z), the distance from the viewer is 0.1.
    // If f is -1000 (meaning -1000 units along -Z), the distance from the viewer is 1000.
    projection = glm::frustum(l, r, b, t, -n, -f);


    // Lighting and material properties
    glm::vec3 lightPos(-4.0f, 4.0f, -3.0f); // Light source position
    glm::vec3 viewPos(0.0f, 0.0f, 0.0f);    // Camera position (eye point e = (0, 0, 0))

    glm::vec3 ka(0.0f, 1.0f, 0.0f);         // Ambient color
    glm::vec3 kd(0.0f, 0.5f, 0.0f);         // Diffuse color
    glm::vec3 ks(0.5f, 0.5f, 0.5f);         // Specular color
    float shininess = 32.0f;                // Shininess exponent (p = 32)
    float Ia = 0.2f;                        // Ambient light intensity

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Process input
        processInput(window);

        // Clear the screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader program
        glUseProgram(shaderProgram);

        // Set uniforms
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
        glUniform3fv(viewPosLoc, 1, glm::value_ptr(viewPos)); // Send camera position

        glUniform3fv(kaLoc, 1, glm::value_ptr(ka));
        glUniform3fv(kdLoc, 1, glm::value_ptr(kd));
        glUniform3fv(ksLoc, 1, glm::value_ptr(ks));
        glUniform1f(shininessLoc, shininess);
        glUniform1f(IaLoc, Ia); // Corrected uniform name

        // Draw the sphere
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, gIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup VBO and shader
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(numBuffers, &GLBuffers[0]);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}