#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Shader.hpp"
#include "Camera.hpp"

#define OK (0)
#define ERR_GLFW (-1)
#define ERR_GLAD (-2)
#define ERR_FILE (-3)
#define ERR_GLSL (-4)
#define ERR_ALLOCATE (-5)

#define WINDOW_WIDTH (800)
#define WINDOW_HEIGHT (600)

#define TERRAIN_WIDTH (100)

// 0.5PI, 4/3 PI, 5/3 PI
// static float vertices[] = {
//   -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
//   0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
//   0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f
// };

// float vertices[] = {
//   -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
//   0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
//   0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
//   0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
//   -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
//   -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
//   -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
//   0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
//   0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
//   0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
//   -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
//   -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
//   -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
//   -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
//   -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
//   -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
//   -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
//   -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
//   0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
//   0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
//   0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
//   0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
//   0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
//   0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
//   -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
//   0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
//   0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
//   0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
//   -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
//   -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
//   -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
//   0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
//   0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
//   0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
//   -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
//   -0.5f, 0.5f, -0.5f, 0.0f, 1.0f
// };

// glm::vec3 cubePositions[] = {
//   glm::vec3(0.0f, 0.0f, 0.0f),
//   glm::vec3(2.0f, 5.0f, -15.0f),
//   glm::vec3(-1.5f, -2.2f, -2.5f),
//   glm::vec3(-3.8f, -2.0f, -12.3f),
//   glm::vec3(2.4f, -0.4f, -3.5f),
//   glm::vec3(-1.7f, 3.0f, -7.5f),
//   glm::vec3(1.3f, -2.0f, -2.5f),
//   glm::vec3(1.5f, 2.0f, -2.5f),
//   glm::vec3(1.5f, 0.2f, -1.5f),
//   glm::vec3(-1.3f, 1.0f, -1.5f)
// };

// float vertices[] = {
//   // positions // colors // texture coords
//   0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
//   0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
//   -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
//   -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // top left
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
// float vertices[] = {
//      0.5f,  0.5f, 0.0f,  // top right
//      0.5f, -0.5f, 0.0f,  // bottom right
//     -0.5f, -0.5f, 0.0f,  // bottom left
//     -0.5f,  0.5f, 0.0f   // top left
// };
// unsigned int indices[] = { // note that we start from 0!
//   0, 1, 3,
//   1, 2, 3
// };

// static long file_length(FILE *file) {
//   if (file == NULL) return -1;
//   long pos = ftell(file);
//   int res = fseek(file, 0, SEEK_END);
//   if (res != 0) return -1;
//   long len = ftell(file);
//   res = fseek(file, pos, SEEK_SET);
//   if (res != 0) return -1;
//   return len;
// }

inline uint32_t random4bytes() {
  return (uint32_t) rand();
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  printf("framebuffer_size_callback(%p, %d, %d)\n", window, width, height);
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
}

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = WINDOW_WIDTH/ 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    printf("Failed to create GLFW window\n");
    glfwTerminate();
    return ERR_GLFW;
  }
  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    return ERR_GLAD;
  }
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  // tell GLFW to capture our mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  Shader shader("resources/vertex2.glsl", "resources/fragment2.glsl");

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
  float colors[10][3] = {
    {1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
    {1.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 1.0f},
    {1.0f, 0.0f, 1.0f},
    {0.5f, 0.0f, 0.0f},
    {1.0f, 0.5f, 0.0f},
    {1.0f, 0.0f, 0.5f},
    {1.0f, 0.5f, 0.5f},
  };
  float (*vertices)[TERRAIN_WIDTH * TERRAIN_WIDTH][2][3] = (float (*)[TERRAIN_WIDTH * TERRAIN_WIDTH][2][3]) malloc(sizeof(*vertices));
  if (vertices == NULL) {
    printf("WHAT\n");
    return ERR_ALLOCATE;
  }
  for (int x = 0; x < TERRAIN_WIDTH; x++) {
    for (int z = 0; z < TERRAIN_WIDTH; z++) {
      uint8_t r = (uint8_t) random4bytes();
      float height = r / 256.0f;
      int base = (x * TERRAIN_WIDTH) + z;
      (*vertices)[base][0][0] = x - (TERRAIN_WIDTH / 2);
      (*vertices)[base][0][1] = height;
      (*vertices)[base][0][2] = z - (TERRAIN_WIDTH / 2);
      (*vertices)[base][1][0] = colors[r % 10][0];
      (*vertices)[base][1][1] = colors[r % 10][1];
      (*vertices)[base][1][2] = colors[r % 10][2];
    }
  }
  int (*indices)[(TERRAIN_WIDTH - 1) * (TERRAIN_WIDTH - 1) * 2][3] = (int (*)[(TERRAIN_WIDTH - 1) * (TERRAIN_WIDTH - 1) * 2][3]) malloc(sizeof(*indices));
  if (indices == NULL) {
    printf("WHAT2\n");
    return ERR_ALLOCATE;
  }
  for (int x = 0; x < (TERRAIN_WIDTH - 1); x++) {
    for (int z = 0; z < (TERRAIN_WIDTH - 1); z++) {
      int base = (x * (TERRAIN_WIDTH - 1) + z) * 2;
      (*indices)[base + 0][0] = (x * TERRAIN_WIDTH) + z;
      (*indices)[base + 0][1] = ((x + 1) * TERRAIN_WIDTH) + z;
      (*indices)[base + 0][2] = ((x + 1) * TERRAIN_WIDTH) + z + 1;
      (*indices)[base + 1][0] = (x * TERRAIN_WIDTH) + z;
      (*indices)[base + 1][1] = (x * TERRAIN_WIDTH) + z + 1;
      (*indices)[base + 1][2] = ((x + 1) * TERRAIN_WIDTH) + z + 1;
    }
  }

  // 2. copy our vertices array in a vertex buffer for OpenGL to use
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(*vertices), vertices, GL_STATIC_DRAW);
  // 3. copy our index array in a element buffer for OpenGL to use
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*indices), indices, GL_STATIC_DRAW);
  // 4. then set the vertex attributes pointers
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  // glEnableVertexAttribArray(2);
  // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
  // glEnableVertexAttribArray(1);

  // cleanup
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  // end cleanup
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
  // glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

  glm::mat4 model = glm::mat4(1.0f);
  model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  glm::mat4 view = glm::mat4(1.0f);
  // note that we're translating the scene in the reverse direction of where we want to move
  view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
  glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

  // // texture time
  // int width, height, numChannels;
  // unsigned char *texture_data = stbi_load("resources/container.jpg", &width, &height, &numChannels, 0);
  // if (texture_data == NULL) {
  //   printf("Failed to load container.jpg\n");
  //   return ERR_FILE;
  // }
  // unsigned int tex1;
  // glGenTextures(1, &tex1);
  // glBindTexture(GL_TEXTURE_2D, tex1);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
  // glGenerateMipmap(GL_TEXTURE_2D);
  // stbi_image_free(texture_data);

  // // second texture time
  // stbi_set_flip_vertically_on_load(true);
  // texture_data = stbi_load("resources/awesomeface.png", &width, &height, &numChannels, 0);
  // if (texture_data == NULL) {
  //   printf("Failed to load container.jpg\n");
  //   return ERR_FILE;
  // }
  // stbi_set_flip_vertically_on_load(false);
  // unsigned int tex2;
  // glGenTextures(1, &tex2);
  // glBindTexture(GL_TEXTURE_2D, tex2);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);
  // glGenerateMipmap(GL_TEXTURE_2D);
  // stbi_image_free(texture_data);

  // glActiveTexture(GL_TEXTURE0);
  // glBindTexture(GL_TEXTURE_2D, tex1);
  // glActiveTexture(GL_TEXTURE1);
  // glBindTexture(GL_TEXTURE_2D, tex2);

  // shader.use();                                                // don't forget to activate the shader before setting uniforms!
  // glUniform1i(glGetUniformLocation(shader.id, "texture1"), 0); // set it manually
  // shader.setInt("texture2", 1);                                // or with shader class
  // // end texture time

  float val = 0.2f;

  glm::mat4 trans = glm::mat4(1.0f);
  trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
  // trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

  glEnable(GL_DEPTH_TEST);

  // auto x = glm::vec3(0.01f, 0.0f, 0.0f);
  // auto y = glm::vec3(0.0f, 0.01f, 0.0f);
  // auto z = glm::vec3(0.0f, 0.0f, 0.01f);

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    // process input
    processInput(window);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
      if (val >= 0.01f) val -= 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
      if (val <= 0.99f) val += 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, deltaTime);
    shader.setFloat("val", val);

    // render
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glUseProgram(shaderProgram);
    // glBindVertexArray(VAO);
    // glDrawArrays(GL_TRIANGLES, 0, 3);
    // glDrawArrays(GL_TRIANGLES, 0, 3);.
    shader.use();
    // model = glm::rotate(model, (float)glfwGetTime() * glm::radians(0.5f), glm::vec3(0.5f, 1.0f, 0.0f));
    int modelLoc = glGetUniformLocation(shader.id, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    // int viewLoc = glGetUniformLocation(shader.id, "view");
    // glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glm::mat4 view = camera.GetViewMatrix();
    shader.setMat4("view", glm::value_ptr(view));
    int projLoc = glGetUniformLocation(shader.id, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    unsigned int transformLoc = glGetUniformLocation(shader.id, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

    glBindVertexArray(VAO);
    // for (unsigned int i = 0; i < 10; i++) {
    //   glm::mat4 model = glm::mat4(1.0f);
    //   model = glm::translate(model, cubePositions[i]);
    //   float angle = 20.0f * i;
    //   model = glm::rotate(model, glm::radians(angle),
    //                       glm::vec3(1.0f, 0.3f, 0.5f));
    //   shader.setMat4("model", glm::value_ptr(model));
    //   glDrawArrays(GL_TRIANGLES, 0, 36);
    // }
    glDrawElements(GL_TRIANGLES, (TERRAIN_WIDTH - 1) * (TERRAIN_WIDTH - 1) * 2 * 3, GL_UNSIGNED_INT, 0);
    // glDrawArrays(GL_TRIANGLES, 0, 36);
    // glm::mat4 trans2 = glm::translate(trans, glm::vec3(-1.0f, 1.0f, 0.0f));
    // glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans2));
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // poll events, swap buffers,
    glfwPollEvents();
    glfwSwapBuffers(window);
  }
  glfwTerminate();
  return OK;
}
