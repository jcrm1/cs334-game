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
#include "Constants.hpp"

#define OK (0)
#define ERR_GLFW (-1)
#define ERR_GLAD (-2)
#define ERR_FILE (-3)
#define ERR_GLSL (-4)
#define ERR_ALLOCATE (-5)
#define ERR_STBI (-6)

#define WINDOW_WIDTH (800)
#define WINDOW_HEIGHT (600)

#define OFFSET(x) (TERRAIN_SCALE_RECIPROCAL - ((x) % TERRAIN_SCALE_RECIPROCAL))

inline uint32_t random4bytes() {
  return (uint32_t)rand();
}

static int window_width = WINDOW_WIDTH;
static int window_height = WINDOW_HEIGHT;
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  printf("framebuffer_size_callback(%p, %d, %d)\n", window, width, height);
  window_width = width;
  window_height = height;
  glViewport(0, 0, width, height);
}

// camera
Camera camera(glm::vec3(0.0f, 20.0f, 0.0f));
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
// static int target_fps = 30;
int main(int argc, char* argv[]) {
  // if (argc == 2) {
  //   int res = sscanf(argv[1], "%d", &target_fps);
  //   if (res != 1) {
  //     printf("Invalid fps\n");
  //   }
  // }
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
  glfwGetFramebufferSize(window, &window_width, &window_height);
  glViewport(0, 0, window_width, window_height);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  // tell GLFW to capture our mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  Shader terrain_shader("resources/terrain_vertex.glsl", "resources/terrain_fragment.glsl");
  Shader godray_shader("resources/godray_vertex.glsl", "resources/godray_fragment.glsl");
  Shader occlusion_shader("resources/terrain_vertex.glsl", "resources/occlusion_fragment.glsl");

  int x, y, numChannels;
  unsigned char *heightmap_data = stbi_load("resources/nashville.png", &x, &y, &numChannels, 1);
  if (heightmap_data == NULL) {
    printf("Error while loading image\n");
    return ERR_STBI;
  }
  printf("Loaded file with %d channels, x: %d, y: %d\n", numChannels, x, y);
  float (*vertices)[2][3] = (float (*)[2][3])malloc((y * x) * sizeof(*vertices));
  if (vertices == NULL) {
    printf("Failed to allocate memory for vertices\n");
    return ERR_ALLOCATE;
  }
  unsigned char heightmap_min = 0xFF;
  unsigned char heightmap_max = 0;
  // here's the plan:
  // 1. want collisions
  // 2. wrote neat algorithm for convex triangle mesh collisions with a rectangular prism
  // 3. algorithm only works when vertices are on integers
  // 4. world space will become 1 unit per heightmap pixel
  // 5. use the model matrix to shrink it down
  // 6. give the camera a very slow speed
  // 7. everything works with no bugs whatsoever. yup
  for (int vz = 0; vz < y; vz++) {
    for (int vx = 0; vx < x; vx++) {
      int base = (vz * x) + vx;
      vertices[base][0][0] = vx;
      vertices[base][0][1] = heightmap_data[base] / 16.0f;
      vertices[base][0][2] = vz;
      if (heightmap_data[base] > heightmap_max) heightmap_max = heightmap_data[base];
      if (heightmap_data[base] < heightmap_min) heightmap_min = heightmap_data[base];
    }
  }
  camera.Position.x = (x / 2) * TERRAIN_SCALE;
  camera.Position.z = (y / 2) * TERRAIN_SCALE;
  stbi_image_free(heightmap_data);
  printf("Created vertices. min: %d max: %d\n", heightmap_min, heightmap_max);
  unsigned int (*indices)[3] = (unsigned int (*)[3])malloc(((y - 1) * (x - 1) * 2) * sizeof(*indices));
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

  unsigned int VAO, VBO, EBO;
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  glGenVertexArrays(1, &VAO);

  glBindVertexArray(VAO);

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

  // screen effects setup begin
  float fullscreen_vertices[] = {
    -1.0f, -1.0f,
    3.0f, -1.0f,
    -1.0f, 3.0f
  };
  unsigned int screen_vao, screen_vbo;
  glGenBuffers(1, &screen_vbo);
  glGenVertexArrays(1, &screen_vao);
  glBindVertexArray(screen_vao);

  glBindBuffer(GL_ARRAY_BUFFER, screen_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(fullscreen_vertices), fullscreen_vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // screen effects setup end

  // cleanup
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  // end cleanup

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glm::mat4 model = glm::mat4(1.0f);
  model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  model = glm::scale(model, glm::vec3(TERRAIN_SCALE, 1.0f, TERRAIN_SCALE));
  glm::mat4 view = glm::mat4(1.0f);
  // note that we're translating the scene in the reverse direction of where we want to move
  view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
  glm::mat4 projection = glm::perspective(glm::radians(45.0f), ((float)window_width) / ((float)window_height), 0.1f, 500.0f);

  // clear color
  glm::vec4 clearColor(0.878f, 0.918f, 0.969f, 1.0f);

  // fog
  float fogStart = 50.0f;
  float fogLength = 25.0f;

  // sun position
  glm::vec3 lightPos(200, 100, 0);

  unsigned int tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  unsigned int fbo;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

  unsigned int rbo;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window_width, window_height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

  unsigned int tex2;
  glGenTextures(1, &tex2);
  glBindTexture(GL_TEXTURE_2D, tex2);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  unsigned int fbo2;
  glGenFramebuffers(1, &fbo2);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo2);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex2, 0);

  unsigned int rbo2;
  glGenRenderbuffers(1, &rbo2);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo2);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window_width, window_height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo2);

  glm::mat4 normalMatrix = glm::transpose(glm::inverse(model));
  unsigned long frameCount = 1;
  // float target_frame_time = 1.0f / target_fps;
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    // if (deltaTime < target_frame_time) usleep();
    lastFrame = currentFrame;
    // process input
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) camera.ProcessKeyboard(DOWN, deltaTime);

    // Process "collisions" but it's really just checking terrain heights
    // glm::vec3 pos = camera.Position;
    // glm::vec3 neg_neg = glm::floor(pos);
    // glm::vec3 pos_pos = glm::ceil(pos);
    int x_scaled_floor = glm::floor(camera.Position.x * TERRAIN_SCALE_RECIPROCAL);
    int x_scaled_ceil = glm::ceil(camera.Position.x * TERRAIN_SCALE_RECIPROCAL);
    int z_scaled_floor = glm::floor(camera.Position.z * TERRAIN_SCALE_RECIPROCAL);
    int z_scaled_ceil = glm::ceil(camera.Position.z * TERRAIN_SCALE_RECIPROCAL);
    int x_offset_floor = x_scaled_floor - OFFSET(x_scaled_floor);
    int x_offset_ceil = x_scaled_floor + OFFSET(x_scaled_floor);
    int z_offset_floor = z_scaled_floor - OFFSET(z_scaled_floor);
    int z_offset_ceil = z_scaled_floor + OFFSET(z_scaled_floor);
    float max_height = 0;
    if (!((x_offset_floor < 0 || x_offset_ceil > x) || (z_offset_floor < 0 || z_offset_ceil > y))) {
      float neg_neg_height = vertices[(z_offset_floor * x) + x_offset_floor][0][1];
      float neg_pos_height = vertices[(z_offset_ceil * x) + x_offset_floor][0][1];
      float pos_pos_height = vertices[(z_offset_ceil * x) + x_offset_ceil][0][1];
      float pos_neg_height = vertices[(z_offset_floor * x) + x_offset_ceil][0][1];
      if (neg_neg_height > max_height) max_height = neg_neg_height;
      if (neg_pos_height > max_height) max_height = neg_pos_height;
      if (pos_pos_height > max_height) max_height = pos_pos_height;
      if (pos_neg_height > max_height) max_height = pos_neg_height;
    }
    max_height += 0.5f;
    if (camera.Position.y < max_height) camera.Position.y = max_height;

    // if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) camera.ModifySpeed(0.1f);
    // if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) camera.ModifySpeed(-0.1f);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) camera.SetSpeed(10.0f);
    else camera.SetSpeed(2.5f);

    // render scene
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, window_width, window_height);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    terrain_shader.use();
    terrain_shader.setMat4("model", glm::value_ptr(model));
    glm::mat4 view = camera.GetViewMatrix();
    terrain_shader.setMat4("view", glm::value_ptr(view));
    terrain_shader.setMat4("projection", glm::value_ptr(projection));
    terrain_shader.setMat4("normalMatrix", glm::value_ptr(normalMatrix));
    terrain_shader.setVec3("cameraPos", camera.Position);
    terrain_shader.setVec4("clearColor", clearColor);
    terrain_shader.setFloat("fogStart", fogStart);
    terrain_shader.setFloat("fogLength", fogLength);
    terrain_shader.setVec3("lightPos", lightPos);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (y - 1) * (x - 1) * 2 * 3, GL_UNSIGNED_INT, 0);

    // render occlusion mask

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo2);
    glViewport(0, 0, window_width, window_height);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    occlusion_shader.use();
    occlusion_shader.setMat4("model", glm::value_ptr(model));
    occlusion_shader.setMat4("view", glm::value_ptr(view));
    occlusion_shader.setMat4("projection", glm::value_ptr(projection));
    occlusion_shader.setMat4("normalMatrix", glm::value_ptr(normalMatrix));
    occlusion_shader.setVec3("cameraPos", camera.Position);
    occlusion_shader.setVec4("clearColor", clearColor);
    occlusion_shader.setFloat("fogStart", fogStart);
    occlusion_shader.setFloat("fogLength", fogLength);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (y - 1) * (x - 1) * 2 * 3, GL_UNSIGNED_INT, 0);

    // render post-processing
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glBindVertexArray(screen_vao);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, window_width, window_height);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex2);
    godray_shader.use();
    godray_shader.setUnsignedInt("screenWidth", window_width);
    godray_shader.setUnsignedInt("screenHeight", window_height);
    // godray_shader.setMat4("model", glm::value_ptr(model));
    godray_shader.setMat4("view", glm::value_ptr(view));
    godray_shader.setMat4("projection", glm::value_ptr(projection));
    godray_shader.setVec4("clearColor", clearColor);
    godray_shader.setInt("prevTex", 0);
    godray_shader.setInt("maskTex", 1);
    godray_shader.setVec3("lightPos", lightPos);
    godray_shader.setFloat("fogStart", fogStart);
    godray_shader.setFloat("fogLength", fogLength);
    godray_shader.setVec3("cameraPos", camera.Position);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // done rendering
    glBindVertexArray(0);

    // poll events, swap buffers,
    glfwPollEvents();
    glfwSwapBuffers(window);
    // printf("Frame %ld\n", frameCount);
    frameCount++;
  }
  glfwTerminate();
  return OK;
}
