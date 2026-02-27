#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define OK (0)
#define ERR_GLFW (-1)
#define ERR_GLAD (-2)
#define ERR_FILE (-3)
#define ERR_GLSL (-4)

#define WINDOW_WIDTH (800)
#define WINDOW_HEIGHT (600)

// 0.5PI, 4/3 PI, 5/3 PI
// static float vertices[] = {
//   -0.5f, -0.5f, 0.0f,
//   0.5f, -0.5f, 0.0f,
//   0.0f,  0.5f, 0.0f
// };

// static float vertices[] = {
//   0.5f, 0.5f, 0.0f, // top right
//   0.5f, -0.5f, 0.0f, // bottom right
//   -0.5f, -0.5f, 0.0f, // bottom left
//   -0.5f, 0.5f, 0.0f // top left
// };

// static unsigned int indices[] = { // note that we start from 0!
//   0, 1, 3, // first triangle
//   1, 2, 3 // second triangle
// };
float vertices[] = {
     0.5f,  0.5f, 0.0f,  // top right
     0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f   // top left 
};
unsigned int indices[] = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
};  

static long file_length(FILE *file) {
  if (file == NULL) return -1;
  long pos = ftell(file);
  int res = fseek(file, 0, SEEK_END);
  if (res != 0) return -1;
  long len = ftell(file);
  res = fseek(file, pos, SEEK_SET);
  if (res != 0) return -1;
  return len;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  printf("framebuffer_size_callback(%p, %d, %d)\n", window, width, height);
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
}

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    printf("Failed to create GLFW window\n");
    glfwTerminate();
    return ERR_GLFW;
  }
  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    return ERR_GLAD;
  }
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  FILE *vertexShaderFile = fopen("resources/vertex.glsl", "r");
  if (vertexShaderFile == NULL) {
    fprintf(stderr, "Unable to open resources/vertex.glsl\n");
    return ERR_FILE;
  }
  int vertexShaderSourceLength = (int) file_length(vertexShaderFile);
  if (vertexShaderSourceLength == -1) {
    fprintf(stderr, "Unable to determine length of resources/vertex.glsl\n");
    return ERR_FILE;
  }
  char *vertexShaderSource = malloc(sizeof(char) * vertexShaderSourceLength);
  fread(vertexShaderSource, vertexShaderSourceLength, 1, vertexShaderFile);

  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, (const char *const *) &vertexShaderSource, &vertexShaderSourceLength);
  glCompileShader(vertexShader);
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    fprintf(stderr, "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    return ERR_GLSL;
  }

  FILE *fragmentShaderFile = fopen("resources/fragment.glsl", "r");
  if (fragmentShaderFile == NULL) {
    fprintf(stderr, "Unable to open resources/fragment.glsl\n");
    return ERR_FILE;
  }
  int fragmentShaderSourceLength = (int) file_length(fragmentShaderFile);
  if (fragmentShaderSourceLength == -1) {
    fprintf(stderr, "Unable to determine length of resources/fragment.glsl\n");
    return ERR_FILE;
  }
  char *fragmentShaderSource = malloc(sizeof(char) * fragmentShaderSourceLength);
  fread(fragmentShaderSource, fragmentShaderSourceLength, 1, fragmentShaderFile);

  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, (const char *const *) &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    fprintf(stderr, "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
    return ERR_GLSL;
  }

  unsigned int shaderProgram = glCreateProgram();

  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    fprintf(stderr, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    return ERR_GLSL;
  }

  glUseProgram(shaderProgram);

  // cleanup
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  fclose(vertexShaderFile);
  vertexShaderFile = NULL;
  fclose(fragmentShaderFile);
  fragmentShaderFile = NULL;
  free(vertexShaderSource);
  vertexShaderSource = NULL;
  free(fragmentShaderSource);
  fragmentShaderSource = NULL;
  // end cleanup

  unsigned int VAO, VBO, EBO;
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  glGenVertexArrays(1, &VAO);
  
  glBindVertexArray(VAO);

  // glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  // // 3. then set our vertex attributes pointers
  // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  // glEnableVertexAttribArray(0);  

  // 2. copy our vertices array in a vertex buffer for OpenGL to use
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  // 3. copy our index array in a element buffer for OpenGL to use
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  // 4. then set the vertex attributes pointers
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);  


  // cleanup
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  // end cleanup
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  while (!glfwWindowShouldClose(window)) {
    // process input
    processInput(window);

    // render
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // glUseProgram(shaderProgram);
    // glBindVertexArray(VAO);
    // glDrawArrays(GL_TRIANGLES, 0, 3);
    // glDrawArrays(GL_TRIANGLES, 0, 3);.
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);


    // poll events, swap buffers, 
    glfwPollEvents();
    glfwSwapBuffers(window);
  }
  glfwTerminate();
  return OK;
}
