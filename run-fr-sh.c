#define GLAD_GL_IMPLEMENTATION
#include "glad/include/glad/glad.h"
#define GLFW_INCLUDE_NONE
#include "glfw/include/GLFW/glfw3.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

char* readFragFile(char* fileName) {
    printf("Fragment file name: %s\n", fileName);
    FILE* f = fopen(fileName, "rb");
    if (f == NULL) {
        char* emsg = "Fragment file ";
        strcat(emsg, fileName);
        strcat(emsg, " not found");
        perror(emsg);
        exit(EXIT_FAILURE);
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* string = malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);

    string[fsize] = 0;
    printf("Fragment shader source:\n%s\n", string);  
    return string;
}

static unsigned int CompileShader(unsigned int type, const char* source) {
    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &source, NULL);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (!result) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char message[length + 1]; // Allocate space for null terminator
        glGetShaderInfoLog(id, length, &length, message);
        message[length] = 0; // Null-terminate the message
        printf("Shader failed to compile: %s\n", message);
        glDeleteShader(id);
        return 0;
    };

    return id;
}

const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "void main() {\n"
    "  gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
    "}\0";

static int CreateShader(char* fragmentShader) {
    unsigned int program = glCreateProgram();

    // Compile vertex shader
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    if (!vs) {
        printf("Failed to compile vertex shader.\n");
        glDeleteProgram(program);
        return 0;
    }
  
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    if (!fs) {
        printf("Failed to compile fragment shader.\n");
        glDeleteShader(vs);
        glDeleteProgram(program);
        return 0;
    }

    glAttachShader(program, vs);
    glAttachShader(program, fs);

    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs); 
    glDeleteShader(fs);

    int success;
    char message[512]; 
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        int length;
        glGetProgramInfoLog(program, 512, &length, message);
        printf("Program failed to link: %s\n", message);
        glDeleteProgram(program);
        return 0;
    }

    return program;
}

int main(int argc, char *argv[]) {
  char* fragmentFileName = argv[1];
  if (argc == 1) {
    printf("Please specify fragment shader file location");
    exit(EXIT_FAILURE);
  }

  glfwSetErrorCallback(error_callback);

  if (!glfwInit())
      exit(EXIT_FAILURE);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(640, 480, "GLSL example", NULL, NULL);
  if (!window) {
      glfwTerminate();
      exit(EXIT_FAILURE);
  }

  glfwSetKeyCallback(window, key_callback);
  
  glfwMakeContextCurrent(window);
  gladLoadGL();
  glfwSwapInterval(1);

  char* fragmentShader = readFragFile(fragmentFileName);
  unsigned int fshader = CreateShader(fragmentShader);
  free(fragmentShader);

  float vertices[] = {
    -1.0f,  1.0f,
    -1.0f, -1.0f,
    1.0f,  -1.0f,
    1.0f, 1.0f
  };

  unsigned int indices[6] = { 0, 1, 2, 2, 3, 0 };

  unsigned int VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
 
  unsigned int ibo;
  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  
  glUseProgram(fshader);

  int uresLocation = glGetUniformLocation(fshader, "u_resolution");
  int uMouseLocation = glGetUniformLocation(fshader, "u_mouse");
  int uTimeLocation = glGetUniformLocation(fshader, "u_time");

  double mxpos;
  double mypos;
  double timeSec = glfwGetTime();

  while (!glfwWindowShouldClose(window)) {
      int width, height;
      glfwGetFramebufferSize(window, &width, &height);
      glfwGetCursorPos(window, &mxpos, &mypos);
      timeSec = glfwGetTime();
      const float ratio = width / (float)height;
      glUniform1f(uTimeLocation, (float)timeSec/10);
      glUniform2f(uresLocation, (float) width, (float) height); 
      glUniform2f(uMouseLocation, (float) mxpos, (float) mypos);
     

      glViewport(0, 0, width, height);
      glClear(GL_COLOR_BUFFER_BIT);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
      
      glfwSwapBuffers(window);
      glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &ibo);
  glDeleteProgram(fshader);
  glfwDestroyWindow(window);

  glfwTerminate();
  exit(EXIT_SUCCESS);
}
