#define GLAD_GL_IMPLEMENTATION
#include "glad/include/glad/glad.h"
#define GLFW_INCLUDE_NONE
#include "glfw/include/GLFW/glfw3.h"

#include <stddef.h>
#include <sys/inotify.h>
#include <limits.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

// =================================================
// GLOBAL VARIABLES
// =================================================

char shouldReloadShader = 0;

// =================================================
// THREAD IMPLEMENTATION
// =================================================

#define iN_EVENT_SIZE sizeof(struct inotify_event) 
#define iN_ALLOCATED_EVENT_SIZE iN_EVENT_SIZE + NAME_MAX + 1
#define iN_EVENT_COUNT_PER_ITERATION 10
#define iN_BUF_LEN (iN_EVENT_COUNT_PER_ITERATION * (iN_ALLOCATED_EVENT_SIZE))

void* hotReloadCheck(void* fragmentFileName) {
  char buf[iN_BUF_LEN];
  int inotify_fd, wd, eCount, irmw, closedFd;
  size_t byte_read, i;
  char fileExists = 1;
  char* pathname = (char*) fragmentFileName;
  struct inotify_event event;

  while (1) {
    inotify_fd = inotify_init();
    if (inotify_fd < 0) {
      perror( "inotify_init" );
      exit(EXIT_FAILURE);
    }

    wd = inotify_add_watch(inotify_fd, pathname, IN_MODIFY | IN_ATTRIB);
    if (wd < 0) {
      perror("inotify_add_watch");
      exit(EXIT_FAILURE);
    }

    byte_read = read(inotify_fd, buf, iN_BUF_LEN);
    if (byte_read < 0) {
      perror( "byte_read" );
      exit(EXIT_FAILURE);
    }

    if (byte_read == 0) continue;
    
    i = 0;
    eCount = 0;
    fileExists = 1;

    while (i < byte_read) {
      memcpy(&event, &buf[i], iN_EVENT_SIZE);
      uint32_t mask = event.mask;
 
      if (mask & IN_ISDIR) {
        printf("HOTRELOAD: CANNOT USE DIRECTORY AS SHADER SOURCE");
        exit(EXIT_FAILURE);
      }
      
      if (mask & IN_MODIFY || mask & IN_ATTRIB) shouldReloadShader = 1;
      if (mask & IN_IGNORED || mask & IN_UNMOUNT || mask & IN_DELETE_SELF) fileExists = 0;

      eCount++;
      if (eCount == iN_EVENT_COUNT_PER_ITERATION) break;
      i += iN_EVENT_SIZE + event.len;
    }
 
    if (fileExists) {
      irmw = inotify_rm_watch( inotify_fd, wd );
      if (irmw < 0) {
        perror( "inotify_rm_watch" );
        exit(EXIT_FAILURE);
      }
    }

    closedFd = close( inotify_fd );
    if (closedFd < 0) {
      perror( "fd close" );
      exit(EXIT_FAILURE);
    }
  }

  return 0;
}

// =================================================
// GLFW - OPENGL LAYER
// =================================================


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
      fprintf(stderr, "Fragment file %s not found: %s\n", fileName, strerror(errno));
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



int uresLocation, uMouseLocation, uTimeLocation;
unsigned int shaderProgram;

void setupShader(char* fragmentFileName) {
  char* fragmentShader = readFragFile(fragmentFileName);
  shaderProgram = CreateShader(fragmentShader);
  free(fragmentShader);
  
  glUseProgram(shaderProgram);

  uresLocation = glGetUniformLocation(shaderProgram, "u_resolution");
  uMouseLocation = glGetUniformLocation(shaderProgram, "u_mouse");
  uTimeLocation = glGetUniformLocation(shaderProgram, "u_time");
}

void setupShaderHotReload(char* fragmentFileName) {
  pthread_t p1;
  pthread_create(&p1, NULL, hotReloadCheck, (void*) fragmentFileName);
}


// =================================================
// MAIN FUNCTION
// =================================================

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
  glfwWindowHint(GLFW_RESIZABLE, 1);

  // Trying to make transparent pixels in linux is just not worth it at all
  // glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 1);

  GLFWwindow* window = glfwCreateWindow(640, 480, "GLSL example", NULL, NULL);
  if (!window) {
      glfwTerminate();
      exit(EXIT_FAILURE);
  }
  
  // if (glfwGetWindowAttrib(window, GLFW_TRANSPARENT_FRAMEBUFFER))
  //  printf("TRANSPARENT WINDOW SUCCESSFULL\n");
  // else printf("TRANSPARENT WINDOW UNSUCCESSFULL!\n");
  


  glfwSetKeyCallback(window, key_callback);
  
  glfwMakeContextCurrent(window);
  gladLoadGL();
  glfwSwapInterval(1);

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
  
  setupShader(fragmentFileName);

  double mxpos;
  double mypos;
  double timeSec = glfwGetTime();

  setupShaderHotReload(fragmentFileName);

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
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
    
    glfwSwapBuffers(window);
    glfwPollEvents();

    if (shouldReloadShader) {
      printf("[HOT RELOADING]\n");
      setupShader(fragmentFileName);
      shouldReloadShader = 0;
    }
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &ibo);
  glDeleteProgram(shaderProgram);
  glfwDestroyWindow(window);

  glfwTerminate();
  exit(EXIT_SUCCESS);
}
