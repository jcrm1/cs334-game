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
#define ERR_STBI (-6)

#define WINDOW_WIDTH (800)
#define WINDOW_HEIGHT (600)

inline uint32_t random4bytes() {
  return (uint32_t)rand();
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  printf("framebuffer_size_callback(%p, %d, %d)\n", window, width, height);
  glViewport(0, 0, width, height);
}

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (firstMouse) {
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
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
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
  int x, y, numChannels;
  unsigned char *heightmap_data = stbi_load("resources/nashville.png", &x, &y, &numChannels, 1);
  if (heightmap_data == NULL) {
    printf("Error while loading image\n");
    return ERR_STBI;
  }
  printf("Loaded file with %d channels, x: %d, y: %d\n", numChannels, x, y);
  float (*vertices)[2][3] = (float (*)[2][3]) malloc((y * x) * sizeof(*vertices));
  if (vertices == NULL) {
    printf("Failed to allocate memory for vertices\n");
    return ERR_ALLOCATE;
  }
  unsigned char heightmap_min = 0xFF;
  unsigned char heightmap_max = 0;
  float xf = x / 100.0f;
  float yf = y / 100.0f;
  for (int vz = 0; vz < y; vz++) {
    for (int vx = 0; vx < x; vx++) {
      int base = (vz * x) + vx;
      vertices[base][0][0] = vx / xf;
      vertices[base][0][1] = heightmap_data[base] / 32.0f;
      vertices[base][0][2] = vz / yf;
      if (heightmap_data[base] > heightmap_max) heightmap_max = heightmap_data[base];
      if (heightmap_data[base] < heightmap_min) heightmap_min = heightmap_data[base];
    }
  }
  stbi_image_free(heightmap_data);
  printf("Created vertices. min: %d max: %d\n", heightmap_min, heightmap_max);
  unsigned int (*indices)[3] = (unsigned int (*)[3]) malloc(((y - 1) * (x - 1) * 2) * sizeof(*indices));
  if (indices == NULL) {
    printf("Failed to allocate memory for indices\n");
    return ERR_ALLOCATE;
  }
  for (int iz = 0; iz < y - 1; iz++) {
    for (int ix = 0; ix < x - 1; ix++) {
      int base = (iz * (x - 1) + ix) * 2;
      // triangle 1
      int tx = (iz * x) + ix;
      int ty = ((iz + 1) * x) + ix;
      int tz = ((iz + 1) * x) + ix + 1;
      if (iz < 5 && ix < 5) printf("iz %d ix %d tx %d ty %d tz %d\n", iz, ix, tx, ty, tz);
      indices[base + 0][0] = tx;
      indices[base + 0][1] = ty;
      indices[base + 0][2] = tz;
      // triangle 1 face normal
      // there's probably some library function to do this stuff and it probably uses vector ops but i dunno how to do that
      float *v0 = vertices[tx][0];
      float *v1 = vertices[ty][0];
      float *v2 = vertices[tz][0];
      glm::vec3 p0(v0[0], v0[1], v0[2]);
      glm::vec3 p1(v1[0], v1[1], v1[2]);
      glm::vec3 p2(v2[0], v2[1], v2[2]);
      // Use swapped cross order so a flat XZ plane points +Y.
      glm::vec3 faceNormal = glm::normalize(glm::cross(p2 - p0, p1 - p0));
      vertices[tx][1][0] += faceNormal.x;
      vertices[tx][1][1] += faceNormal.y;
      vertices[tx][1][2] += faceNormal.z;
      vertices[ty][1][0] += faceNormal.x;
      vertices[ty][1][1] += faceNormal.y;
      vertices[ty][1][2] += faceNormal.z;
      vertices[tz][1][0] += faceNormal.x;
      vertices[tz][1][1] += faceNormal.y;
      vertices[tz][1][2] += faceNormal.z;

      // triangle 2
      tx = (iz * x) + ix;
      ty = (iz * x) + ix + 1;
      tz = ((iz + 1) * x) + ix + 1;
      indices[base + 1][0] = tx;
      indices[base + 1][1] = ty;
      indices[base + 1][2] = tz;
      // triangle 2 face normal
      v0 = vertices[tx][0];
      v1 = vertices[ty][0];
      v2 = vertices[tz][0];
      p0 = glm::vec3(v0[0], v0[1], v0[2]);
      p1 = glm::vec3(v1[0], v1[1], v1[2]);
      p2 = glm::vec3(v2[0], v2[1], v2[2]);
      faceNormal = glm::normalize(glm::cross(p2 - p0, p1 - p0));
      vertices[tx][1][0] += faceNormal.x;
      vertices[tx][1][1] += faceNormal.y;
      vertices[tx][1][2] += faceNormal.z;
      vertices[ty][1][0] += faceNormal.x;
      vertices[ty][1][1] += faceNormal.y;
      vertices[ty][1][2] += faceNormal.z;
      vertices[tz][1][0] += faceNormal.x;
      vertices[tz][1][1] += faceNormal.y;
      vertices[tz][1][2] += faceNormal.z;
    }
  }
  printf("Created indices\n");
  // normalize normals
  for (int i = 0; i < y * x; i++) {
    float *normal = vertices[i][1];
    float len = sqrtf((normal[0] * normal[0]) + (normal[1] * normal[1]) + (normal[2] * normal[2]));
    normal[0] /= len;
    normal[1] /= len;
    normal[2] /= len;
  }
  printf("Normalized normals\n");

  // float colors[10][3] = {
  //   {1.0f, 0.0f, 0.0f},
  //   {0.0f, 1.0f, 0.0f},
  //   {0.0f, 0.0f, 1.0f},
  //   {1.0f, 1.0f, 0.0f},
  //   {0.0f, 1.0f, 1.0f},
  //   {1.0f, 0.0f, 1.0f},
  //   {0.5f, 0.0f, 0.0f},
  //   {1.0f, 0.5f, 0.0f},
  //   {1.0f, 0.0f, 0.5f},
  //   {1.0f, 0.5f, 0.5f},
  // };

  // 2. copy our vertices array in a vertex buffer for OpenGL to use
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, (y * x) * sizeof(*vertices), vertices, GL_STATIC_DRAW);
  // 3. copy our index array in a element buffer for OpenGL to use
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, ((y - 1) * (x - 1) * 2) * sizeof(*indices), indices, GL_STATIC_DRAW);
  // 4. then set the vertex attributes pointers
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

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

  glEnable(GL_DEPTH_TEST);

  // auto x = glm::vec3(0.01f, 0.0f, 0.0f);
  // auto y = glm::vec3(0.0f, 0.01f, 0.0f);
  // auto z = glm::vec3(0.0f, 0.0f, 0.01f);

  glm::mat4 normalMatrix = glm::transpose(glm::inverse(model));
  unsigned long frameCount = 1;
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    // process input
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) camera.ProcessKeyboard(DOWN, deltaTime);
    // if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) camera.ModifySpeed(0.1f);
    // if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) camera.ModifySpeed(-0.1f);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) camera.SetSpeed(5.0f);
    else camera.SetSpeed(2.5f);

    // render
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glUseProgram(shaderProgram);
    // glBindVertexArray(VAO);
    // glDrawArrays(GL_TRIANGLES, 0, 3);
    // glDrawArrays(GL_TRIANGLES, 0, 3);.
    shader.use();
    shader.setMat4("model", glm::value_ptr(model));
    glm::mat4 view = camera.GetViewMatrix();
    shader.setMat4("view", glm::value_ptr(view));
    shader.setMat4("projection", glm::value_ptr(projection));
    shader.setMat4("normalMatrix", glm::value_ptr(normalMatrix));

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
    glDrawElements(GL_TRIANGLES, (y - 1) * (x - 1) * 2 * 3, GL_UNSIGNED_INT, 0);
    // glDrawArrays(GL_TRIANGLES, 0, 36);
    // glm::mat4 trans2 = glm::translate(trans, glm::vec3(-1.0f, 1.0f, 0.0f));
    // glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans2));
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // poll events, swap buffers,
    glfwPollEvents();
    glfwSwapBuffers(window);
    printf("Frame %ld\n", frameCount);
    frameCount++;
  }
  glfwTerminate();
  return OK;
}
