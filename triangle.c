#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#define GLAD_GL_IMPLEMENTATION // Necessary for headeronly version.
#include "glad.h"
#include "glfw3.h"


const GLuint WIDTH = 800, HEIGHT = 600;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    printf("PRESSED %d", key);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

#define GLAD_VERSION_MAJOR(version) (version / 10000)
#define GLAD_VERSION_MINOR(version) (version % 10000)



const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const GLchar* fragmentShaderSource = "#version 330 core\n"
  "out vec4 color;\n"
  "void main()\n"
  "{\n"
  "color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
  "}\n\0";


int main(void) {
    // Create GLFW window ==============

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "404's First triangle yeppi", NULL, NULL);
    
    printf("2\n");

    if (window == NULL) {
      printf("Failed to create GLFW window! \n");
      return -1;
    }

    glfwMakeContextCurrent(window);
    
    // ======================
    printf("3\n");

    // call framebuffer_size_callback function whenever window size changes
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // assaign a function to listen keyboard events
    glfwSetKeyCallback(window, key_callback);

    printf("4\n");
    int version = gladLoadGL();
    printf("GL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

    // ========================== Build and compile the shader program
    
    printf("5\n");
    
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

    printf("6\n");
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);

    printf("7\n");
    glCompileShader(vertexShader);

    printf("1");
 
    // check for linking errors
    GLint vertexCompiled;
    GLchar vertexCompiledInfo[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexCompiled);
    if (!vertexCompiled) {
      glGetShaderInfoLog(vertexShader, 512, NULL, vertexCompiledInfo);
      printf("\nVERTEX SHADER COMPILATION FAILED\n %s", vertexCompiledInfo);
      return -1;
    }


    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // check for linking errors
    GLint fragmentCompiled;
    GLchar fragmentCompileInfo[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentCompiled);
    if (!fragmentCompiled) { 
      glGetShaderInfoLog(fragmentShader, 512, NULL, fragmentCompileInfo);
      printf("\nFRAGMENT SHADER COMPILATION FAILED\n %s", fragmentCompileInfo);
      return -1;
    }



    // link the shaders
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    GLint shadersLinkStatus;
    GLchar shadersLinkInfo[512];
    		
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &shadersLinkStatus);
    if (!shadersLinkStatus) {
      glGetProgramInfoLog(shaderProgram, 512, NULL, shadersLinkInfo);
      printf("Couldn't link shaders to the shader program %s \n", shadersLinkInfo);
      return -1;  
    }

    printf("Shaders link status: %d", shadersLinkStatus);

    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);



    // ==========================
 

    float vertices[] = {
      -0.5f, -0.5f, 0.0f,
      0.5f, -0.5f, 0.0f,
      0.0f, -0.5f, 0.0f
    };

    
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
 

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);    

        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return 0;
}

