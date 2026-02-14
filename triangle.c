#define GLAD_GL_IMPLEMENTATION
#include "glad/include/glad/glad.h"
#define GLFW_INCLUDE_NONE
#include "glfw/include/GLFW/glfw3.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
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
    char message[length];
    glGetShaderInfoLog(id, length, &length, message);
    printf("Shader failed to compile %s\n", message);
    glDeleteShader(id);
    return 0;
  };

  return id;
}

static int CreateShader(char* vertexShader, char* fragmentShader) {
  unsigned int program = glCreateProgram();
  unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
  unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

  glAttachShader(program, vs);
  glAttachShader(program, fs);

  glLinkProgram(program);
  glValidateProgram(program);

  glDeleteShader(vs);
  glDeleteShader(fs);

  int success;
  char *message;

  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    int length;
    glGetProgramInfoLog(program, 512, &length, message);
    printf("Program failed: %s\n", message);
    return 0;
  }

  return program;
}

int main(void)
{
  glfwSetErrorCallback(error_callback);

  if (!glfwInit())
      exit(EXIT_FAILURE);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGL Triangle", NULL, NULL);
  if (!window)
  {
      glfwTerminate();
      exit(EXIT_FAILURE);
  }

  glfwSetKeyCallback(window, key_callback);

  glfwMakeContextCurrent(window);
  gladLoadGL();
  glfwSwapInterval(1);

  float positions[6] = {
    -0.5f, -0.5f,
    0.0f, 0.5f, 
    0.5f, -0.5f
  };

  GLuint vertex_array;
  glGenVertexArrays(1, &vertex_array);
  glBindVertexArray(vertex_array);
  glEnableVertexAttribArray(0);

  unsigned int buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);

  glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);
  
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8, 0);

  char* vertexShader = "#version 330 core\n" 
    "layout(location = 0) in vec4 position;\n"
    "void main()\n" 
    "{\n"
    "gl_Position = position; \n" 
    "}\n";

  char* fragmentShader  = "#version 330 core\n" 
    "layout(location = 0) out vec4 color;\n"
    "void main()\n" 
    "{\n"
    "color = vec4(1.0, 0.0, 0.0, 1.0); \n" 
    "}\n";


  unsigned int shader = CreateShader(vertexShader, fragmentShader);
  glUseProgram(shader);
  
  while (!glfwWindowShouldClose(window))
  {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    const float ratio = width / (float) height;

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, 3);    
    
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteProgram(shader);
  glfwDestroyWindow(window);

  glfwTerminate();
  exit(EXIT_SUCCESS);
}
