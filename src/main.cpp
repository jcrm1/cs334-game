#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "FastNoiseLite.h"

#include "Vertex.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Constants.hpp"
#include "Water.hpp"



#define OK (0)
#define ERR_GLFW (-1)
#define ERR_GLAD (-2)
#define ERR_FILE (-3)
#define ERR_GLSL (-4)
#define ERR_ALLOCATE (-5)

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
  Shader water_shader("resources/water_vertex.glsl", "resources/water_fragment.glsl");

  //int x = 512, y = 512;
  int x = 1024, y = 1024;

  //VERTEX ARRAY
  Vertex *vertices = (Vertex *)calloc((y * x), sizeof(Vertex));
  if (vertices == NULL) {
    printf("Failed to allocate memory for vertices\n");
    return ERR_ALLOCATE;
  }

  // Biome selector — low frequency so biome regions are large and gradual
  FastNoiseLite biome;
  biome.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
  biome.SetFractalType(FastNoiseLite::FractalType_FBm);
  biome.SetFractalOctaves(2);
  biome.SetFractalLacunarity(2.0f);
  biome.SetFractalGain(0.5f);
  biome.SetFrequency(0.003f);

  // Plains — very low frequency, few octaves, almost no relief
  FastNoiseLite plains_noise;
  plains_noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
  plains_noise.SetFractalType(FastNoiseLite::FractalType_FBm);
  plains_noise.SetFractalOctaves(3);
  plains_noise.SetFractalLacunarity(2.0f);
  plains_noise.SetFractalGain(0.5f);
  plains_noise.SetFrequency(0.0001f);

  FastNoiseLite plains_erosion;
  plains_erosion.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
  plains_erosion.SetFractalType(FastNoiseLite::FractalType_FBm);
  plains_erosion.SetFractalOctaves(3);
  plains_erosion.SetFractalLacunarity(2.0f);
  plains_erosion.SetFractalGain(0.4f);
  plains_erosion.SetFrequency(0.008f);

  // Forest — medium frequency, moderate octaves, rolling hills
  FastNoiseLite forest_noise;
  forest_noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
  forest_noise.SetFractalType(FastNoiseLite::FractalType_FBm);
  forest_noise.SetFractalOctaves(5);
  forest_noise.SetFractalLacunarity(2.0f);
  forest_noise.SetFractalGain(0.5f);
  forest_noise.SetFrequency(0.001f);

  FastNoiseLite forest_erosion;
  forest_erosion.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
  forest_erosion.SetFractalType(FastNoiseLite::FractalType_FBm);
  forest_erosion.SetFractalOctaves(5);
  forest_erosion.SetFractalLacunarity(2.2f);
  forest_erosion.SetFractalGain(0.3f);
  forest_erosion.SetFrequency(0.0002f);

  // Mountains — higher frequency, many octaves, high lacunarity for jagged peaks
  FastNoiseLite mountain_noise;
  mountain_noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
  mountain_noise.SetFractalType(FastNoiseLite::FractalType_FBm);
  mountain_noise.SetFractalOctaves(8);
  mountain_noise.SetFractalLacunarity(2.6f);
  mountain_noise.SetFractalGain(0.5f);
  mountain_noise.SetFrequency(0.0007f);

  FastNoiseLite mountain_erosion;
  mountain_erosion.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
  mountain_erosion.SetFractalType(FastNoiseLite::FractalType_FBm);
  mountain_erosion.SetFractalOctaves(6);
  mountain_erosion.SetFractalLacunarity(2.4f);
  mountain_erosion.SetFractalGain(0.25f);
  mountain_erosion.SetFrequency(0.003f);

  // here's the plan:
  // 1. want collisions
  // 2. wrote neat algorithm for convex triangle mesh collisions with a rectangular prism
  // 3. algorithm only works when vertices are on integers
  // 4. world space will become 1 unit per heightmap pixel
  // 5. use the model matrix to shrink it down
  // 6. give the camera a very slow speed
  // 7. everything works with no bugs whatsoever. yup
  float heightmap_min = FLT_MAX;
  float heightmap_max = -FLT_MAX;
  for (int vz = 0; vz < y; vz++) {
    for (int vx = 0; vx < x; vx++) {
      int base = (vz * x) + vx;
      float fvx = (float)vx, fvz = (float)vz;
      float b = biome.GetNoise(fvx, fvz); // [-1, 1]: -1 = plains, 0 = forest, +1 = mountains

      // Triangular basis weights — each biome peaks at a point and tapers linearly.
      // b=-1 → plains, b=0 → forest, b=+1 → mountains. Always sums to 1.
      float t = (b + 1.0f) * 0.5f; // remap [-1,1] to [0,1]
      float w_plains   = glm::clamp(1.0f - t * 2.0f, 0.0f, 1.0f);
      float w_mountain = glm::clamp(t * 2.0f - 1.0f, 0.0f, 1.0f);
      float w_forest   = 1.0f - w_plains - w_mountain;

      float h_plains   = (plains_noise.GetNoise(fvx, fvz)   - plains_erosion.GetNoise(fvx, fvz)   * 0.15f + 1.0f) * 2.5f;
      float h_forest   = (forest_noise.GetNoise(fvx, fvz)   - forest_erosion.GetNoise(fvx, fvz)   * 0.20f + 1.0f) * 4.5f;
      float h_mountain = (mountain_noise.GetNoise(fvx, fvz) - mountain_erosion.GetNoise(fvx, fvz) * 0.35f + 1.0f) * 8.0f;

      float height = w_plains * h_plains + w_forest * h_forest + w_mountain * h_mountain;

      // Flatten terrain below sea level, but skip mountain zones so they keep full relief.
      t = 1.0f - glm::smoothstep(SEA_LEVEL - 3.0f, SEA_LEVEL, height);
      t *= (1.0f - w_mountain);
      height = glm::mix(height, SEA_LEVEL - 0.5f, t);

      vertices[base].position[0] = vx;
      vertices[base].position[1] = height;
      vertices[base].position[2] = vz;
      if (height > heightmap_max) heightmap_max = height;
      if (height < heightmap_min) heightmap_min = height;
    }
  }
  camera.Position.x = (x / 2) * TERRAIN_SCALE;
  camera.Position.z = (y / 2) * TERRAIN_SCALE;
  printf("Generated terrain. min: %.2f max: %.2f\n", heightmap_min, heightmap_max);
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
      float *v0 = vertices[tx].position;
      float *v1 = vertices[ty].position;
      float *v2 = vertices[tz].position;
      glm::vec3 p0(v0[0], v0[1], v0[2]);
      glm::vec3 p1(v1[0], v1[1], v1[2]);
      glm::vec3 p2(v2[0], v2[1], v2[2]);
      // Use swapped cross order so a flat XZ plane points +Y.
      glm::vec3 faceNormal = glm::normalize(glm::cross(p2 - p0, p1 - p0));
      vertices[tx].normal[0] += faceNormal.x;
      vertices[tx].normal[1] += faceNormal.y;
      vertices[tx].normal[2] += faceNormal.z;
      vertices[ty].normal[0] += faceNormal.x;
      vertices[ty].normal[1] += faceNormal.y;
      vertices[ty].normal[2] += faceNormal.z;
      vertices[tz].normal[0] += faceNormal.x;
      vertices[tz].normal[1] += faceNormal.y;
      vertices[tz].normal[2] += faceNormal.z;

      // triangle 2
      tx = (iz * x) + ix;
      ty = (iz * x) + ix + 1;
      tz = ((iz + 1) * x) + ix + 1;
      indices[base + 1][0] = tx;
      indices[base + 1][1] = ty;
      indices[base + 1][2] = tz;
      // triangle 2 face normal
      v0 = vertices[tx].position;
      v1 = vertices[ty].position;
      v2 = vertices[tz].position;
      p0 = glm::vec3(v0[0], v0[1], v0[2]);
      p1 = glm::vec3(v1[0], v1[1], v1[2]);
      p2 = glm::vec3(v2[0], v2[1], v2[2]);
      faceNormal = glm::normalize(glm::cross(p2 - p0, p1 - p0));
      vertices[tx].normal[0] += faceNormal.x;
      vertices[tx].normal[1] += faceNormal.y;
      vertices[tx].normal[2] += faceNormal.z;
      vertices[ty].normal[0] += faceNormal.x;
      vertices[ty].normal[1] += faceNormal.y;
      vertices[ty].normal[2] += faceNormal.z;
      vertices[tz].normal[0] += faceNormal.x;
      vertices[tz].normal[1] += faceNormal.y;
      vertices[tz].normal[2] += faceNormal.z;
    }
  }
  printf("Created indices\n");
  // normalize normals
  for (int i = 0; i < y * x; i++) {
    float *normal = vertices[i].normal;
    float len = sqrtf((normal[0] * normal[0]) + (normal[1] * normal[1]) + (normal[2] * normal[2]));
    normal[0] /= len;
    normal[1] /= len;
    normal[2] /= len;
  }
  printf("Normalized normals\n");

  int source = 524800;
  int amount = 50000;
  int *waterBottom = (int *)malloc(amount * sizeof(int));
  if (waterBottom == NULL) {
    printf("Failed to allocate memory for waterBottom\n");
    return ERR_ALLOCATE;
  }
  createBottomMesh(&waterBottom, source, amount, (vertices), indices, (y - 1) * (x - 1) * 2, x*y);
  printf("Created water bottom mesh\n");

  // printtriangles(waterBottom, amount);

  //get triangle indices and vertices for water bottom mesh

  unsigned int (*waterBottomIndices)[3] = (unsigned int (*)[3])malloc((amount * 3) * sizeof(*waterBottomIndices));
  printf("sizeof waterBottomIndices: %d\n", int(sizeof(*waterBottomIndices)));
  if (waterBottomIndices == NULL) {
    printf("Failed to allocate memory for waterBottomIndices\n");
    return ERR_ALLOCATE;
  }
  printf("Allocated waterBottomIndices\n");
  Vertex *waterBottomVertices = (Vertex *)malloc((amount * 3) * sizeof(Vertex));
  printf("sizeof waterBottomVertices: %d\n", int(sizeof(Vertex)));
  if (waterBottomVertices == NULL) {
    printf("Failed to allocate memory for waterBottomVertices\n");
    return ERR_ALLOCATE;
  }
  printf("Allocated waterBottomVertices\n");
  for (int i = 0; i < amount; i++){
    int triIndex = waterBottom[i];
    // printf("W %d: %d\n", i, triIndex);
    waterBottomIndices[i][0] = indices[triIndex][0];
    waterBottomIndices[i][1] = indices[triIndex][1];
    waterBottomIndices[i][2] = indices[triIndex][2];
    waterBottomVertices[i*3 + 0] = vertices[indices[triIndex][0]];
    waterBottomVertices[i*3 + 1] = vertices[indices[triIndex][1]];
    waterBottomVertices[i*3 + 2] = vertices[indices[triIndex][2]];
  }
  printf("Copied water bottom mesh data\n");

  /*
   * LOD plan:
   * 1. generate 1/2 density mesh. maybe use lowest height on exterior?
   * 2. in pass 1, render pixels up to distance d1
   * 3. in pass 2, render pixels past distance d1
   */
  // generate 1/2 density mesh
  int lod_1_x = (int) glm::ceil(x / 2.0);
  int lod_1_z = (int) glm::ceil(y / 2.0);
  Vertex *lod_1_verts = (Vertex *) calloc((lod_1_x * lod_1_z), sizeof(Vertex));
  if (lod_1_verts == NULL) {
    printf("Failed to allocate memory for lod_1_verts\n");
    return ERR_ALLOCATE;
  }
  printf("Allocated lod_1_verts\n");
  for (int vz = 0; vz < lod_1_z; vz++) {
    for (int vx = 0; vx < lod_1_x; vx++) {
      int ox0 = vx * 2;
      int oz0 = vz * 2;
      if (ox0 > x - 1) ox0 = x - 1;
      if (oz0 > y - 1) oz0 = y - 1;
      int ox1 = ox0 + 1;
      int oz1 = oz0 + 1;
      if (ox1 > x - 1) ox1 = x - 1;
      if (oz1 > y - 1) oz1 = y - 1;

      float *p00 = vertices[(oz0 * x) + ox0].position;
      float *p10 = vertices[(oz0 * x) + ox1].position;
      float *p01 = vertices[(oz1 * x) + ox0].position;
      float *p11 = vertices[(oz1 * x) + ox1].position;

      int out = (vz * lod_1_x) + vx;
      lod_1_verts[out].position[0] = p00[0];
      lod_1_verts[out].position[1] = (p00[1] + p10[1] + p01[1] + p11[1]) / 4.0f;
      lod_1_verts[out].position[2] = p00[2];
    }
  }
  unsigned int (*lod_1_indices)[3] = (unsigned int (*)[3])malloc(((lod_1_x - 1) * (lod_1_z - 1) * 2) * sizeof(*lod_1_indices));
  if (lod_1_indices == NULL) {
    printf("Failed to allocate memory for lod_1_indices\n");
    return ERR_ALLOCATE;
  }
  for (int iz = 0; iz < lod_1_z - 1; iz++) {
    for (int ix = 0; ix < lod_1_x - 1; ix++) {
      int base = (iz * (lod_1_x - 1) + ix) * 2;
      // triangle 1
      int tx = (iz * lod_1_x) + ix;
      int ty = ((iz + 1) * lod_1_x) + ix;
      int tz = ((iz + 1) * lod_1_x) + ix + 1;
      lod_1_indices[base + 0][0] = tx;
      lod_1_indices[base + 0][1] = ty;
      lod_1_indices[base + 0][2] = tz;
      // triangle 1 face normal
      // there's probably some library function to do this stuff and it probably uses vector ops but i dunno how to do that
      float *v0 = lod_1_verts[tx].position;
      float *v1 = lod_1_verts[ty].position;
      float *v2 = lod_1_verts[tz].position;
      glm::vec3 p0(v0[0], v0[1], v0[2]);
      glm::vec3 p1(v1[0], v1[1], v1[2]);
      glm::vec3 p2(v2[0], v2[1], v2[2]);
      // Use swapped cross order so a flat XZ plane points +Y.
      glm::vec3 faceNormal = glm::normalize(glm::cross(p2 - p0, p1 - p0));
      lod_1_verts[tx].normal[0] += faceNormal.x;
      lod_1_verts[tx].normal[1] += faceNormal.y;
      lod_1_verts[tx].normal[2] += faceNormal.z;
      lod_1_verts[ty].normal[0] += faceNormal.x;
      lod_1_verts[ty].normal[1] += faceNormal.y;
      lod_1_verts[ty].normal[2] += faceNormal.z;
      lod_1_verts[tz].normal[0] += faceNormal.x;
      lod_1_verts[tz].normal[1] += faceNormal.y;
      lod_1_verts[tz].normal[2] += faceNormal.z;

      // triangle 2
      tx = (iz * lod_1_x) + ix;
      ty = (iz * lod_1_x) + ix + 1;
      tz = ((iz + 1) * lod_1_x) + ix + 1;
      lod_1_indices[base + 1][0] = tx;
      lod_1_indices[base + 1][1] = ty;
      lod_1_indices[base + 1][2] = tz;
      // triangle 2 face normal
      v0 = lod_1_verts[tx].position;
      v1 = lod_1_verts[ty].position;
      v2 = lod_1_verts[tz].position;
      p0 = glm::vec3(v0[0], v0[1], v0[2]);
      p1 = glm::vec3(v1[0], v1[1], v1[2]);
      p2 = glm::vec3(v2[0], v2[1], v2[2]);
      faceNormal = glm::normalize(glm::cross(p2 - p0, p1 - p0));
      lod_1_verts[tx].normal[0] += faceNormal.x;
      lod_1_verts[tx].normal[1] += faceNormal.y;
      lod_1_verts[tx].normal[2] += faceNormal.z;
      lod_1_verts[ty].normal[0] += faceNormal.x;
      lod_1_verts[ty].normal[1] += faceNormal.y;
      lod_1_verts[ty].normal[2] += faceNormal.z;
      lod_1_verts[tz].normal[0] += faceNormal.x;
      lod_1_verts[tz].normal[1] += faceNormal.y;
      lod_1_verts[tz].normal[2] += faceNormal.z;
    }
  }
  // lod 1 normalize normals
  for (int i = 0; i < lod_1_z * lod_1_x; i++) {
    float *normal = lod_1_verts[i].normal;
    float len = sqrtf((normal[0] * normal[0]) + (normal[1] * normal[1]) + (normal[2] * normal[2]));
    if (len > 0.00001f) {
      normal[0] /= len;
      normal[1] /= len;
      normal[2] /= len;
    } else {
      normal[0] = 0.0f;
      normal[1] = 1.0f;
      normal[2] = 0.0f;
    }
  }
  printf("Generated LOD mesh\n");

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
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
  glEnableVertexAttribArray(1);
  printf("Created terrain VAO\n");


  // water bottom mesh (non-functional)

  unsigned int VAO_WATER, VBO_WATER, EBO_WATER;
  glGenBuffers(1, &VBO_WATER);
  glGenBuffers(1, &EBO_WATER);
  glGenVertexArrays(1, &VAO_WATER);

  glBindVertexArray(VAO_WATER);

  // 2. copy our vertices array in a vertex buffer for OpenGL to use
  glBindBuffer(GL_ARRAY_BUFFER, VBO_WATER);
  glBufferData(GL_ARRAY_BUFFER, (y * x) * sizeof(*vertices), vertices, GL_STATIC_DRAW);
  // 3. copy our index array in a element buffer for OpenGL to use
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_WATER);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, amount * 3 * sizeof(*waterBottomIndices), waterBottomIndices, GL_STATIC_DRAW);
  // 4. then set the vertex attributes pointers
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
  glEnableVertexAttribArray(1);

  unsigned int VAO_LOD, VBO_LOD, EBO_LOD;
  glGenBuffers(1, &VBO_LOD);
  glGenBuffers(1, &EBO_LOD);
  glGenVertexArrays(1, &VAO_LOD);

  glBindVertexArray(VAO_LOD);

  // 2. copy our vertices array in a vertex buffer for OpenGL to use
  glBindBuffer(GL_ARRAY_BUFFER, VBO_LOD);
  glBufferData(GL_ARRAY_BUFFER, (lod_1_z * lod_1_x) * sizeof(*lod_1_verts), lod_1_verts, GL_STATIC_DRAW);
  // 3. copy our index array in a element buffer for OpenGL to use
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_LOD);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, ((lod_1_z - 1) * (lod_1_x - 1) * 2) * sizeof(*lod_1_indices), lod_1_indices, GL_STATIC_DRAW);
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

  glEnable(GL_BLEND);

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

  unsigned int terrain_tex;
  glGenTextures(1, &terrain_tex);
  glBindTexture(GL_TEXTURE_2D, terrain_tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  unsigned int terrain_fbo;
  glGenFramebuffers(1, &terrain_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, terrain_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, terrain_tex, 0);

  unsigned int terrain_rbo;
  glGenRenderbuffers(1, &terrain_rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, terrain_rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window_width, window_height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, terrain_rbo);

  unsigned int mask_tex;
  glGenTextures(1, &mask_tex);
  glBindTexture(GL_TEXTURE_2D, mask_tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  unsigned int mask_fbo;
  glGenFramebuffers(1, &mask_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, mask_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mask_tex, 0);

  unsigned int mask_rbo;
  glGenRenderbuffers(1, &mask_rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, mask_rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window_width, window_height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mask_rbo);

  unsigned int terrain_lod_tex;
  glGenTextures(1, &terrain_lod_tex);
  glBindTexture(GL_TEXTURE_2D, terrain_lod_tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  unsigned int terrain_lod_fbo;
  glGenFramebuffers(1, &terrain_lod_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, terrain_lod_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, terrain_lod_tex, 0);

  unsigned int terrain_lod_rbo;
  glGenRenderbuffers(1, &terrain_lod_rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, terrain_lod_rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window_width, window_height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, terrain_lod_rbo);

  glm::mat4 normalMatrix = glm::transpose(glm::inverse(model));
  unsigned long frameCount = 1;
  // float target_frame_time = 1.0f / target_fps;
  bool grounded = false;
  bool fogKeyDown = false;
  bool enableFog = true;
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
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !fogKeyDown) {
      enableFog = !enableFog;
      fogKeyDown = true;
    } else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
      fogKeyDown = false;
    }

    // Process "collisions" but it's really just checking terrain heights
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
      float neg_neg_height = vertices[(z_offset_floor * x) + x_offset_floor].position[1];
      float neg_pos_height = vertices[(z_offset_ceil * x) + x_offset_floor].position[1];
      float pos_pos_height = vertices[(z_offset_ceil * x) + x_offset_ceil].position[1];
      float pos_neg_height = vertices[(z_offset_floor * x) + x_offset_ceil].position[1];
      if (neg_neg_height > max_height) max_height = neg_neg_height;
      if (neg_pos_height > max_height) max_height = neg_pos_height;
      if (pos_pos_height > max_height) max_height = pos_pos_height;
      if (pos_neg_height > max_height) max_height = pos_neg_height;
    }
    max_height += 0.5f;
    if (camera.Position.y > max_height && glfwGetKey(window, GLFW_KEY_SPACE) != GLFW_PRESS) camera.ProcessKeyboard(DOWN, deltaTime);
    if (camera.Position.y < max_height) camera.Position.y = max_height;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) camera.SetSpeed(10.0f);
    else camera.SetSpeed(2.5f);

    // render scene
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glBindFramebuffer(GL_FRAMEBUFFER, terrain_fbo);
    glViewport(0, 0, window_width, window_height);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    terrain_shader.use();
    terrain_shader.setMat4("model", glm::value_ptr(model));
    glm::mat4 view = camera.GetViewMatrix();
    terrain_shader.setMat4("view", glm::value_ptr(view));
    terrain_shader.setMat4("projection", glm::value_ptr(projection));
    terrain_shader.setVec3("cameraPos", camera.Position);
    terrain_shader.setMat4("normalMatrix", glm::value_ptr(normalMatrix));
    terrain_shader.setVec4("clearColor", clearColor);
    terrain_shader.setFloat("fogStart", fogStart);
    terrain_shader.setFloat("fogLength", fogLength);
    terrain_shader.setFloat("seaLevel", SEA_LEVEL);
    terrain_shader.setVec3("lightPos", lightPos);
    terrain_shader.setBool("enableFog", enableFog);

    // render far
    if (!enableFog) {
      terrain_shader.setBool("near", false);
      glBindVertexArray(VAO_LOD);
      glDrawElements(GL_TRIANGLES, (lod_1_z - 1) * (lod_1_x - 1) * 2 * 3, GL_UNSIGNED_INT, 0);
    }
    
    // render near
    terrain_shader.setBool("near", true);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (y - 1) * (x - 1) * 2 * 3, GL_UNSIGNED_INT, 0);

    // printf("Rendering water\n");
    water_shader.use();
    // printf("This worked\n");
    water_shader.setMat4("model", glm::value_ptr(model));
    // printf("Set water model matrix\n");
    water_shader.setMat4("view", glm::value_ptr(view));
    // printf("Set water view matrix\n");
    water_shader.setMat4("projection", glm::value_ptr(projection));
    // printf("Set water projection matrix\n");
    water_shader.setVec3("cameraPos", camera.Position);
    // printf("Set water camera position\n");
    water_shader.setVec4("clearColor", clearColor);
    // printf("Set water clear color\n");
    water_shader.setFloat("fogStart", fogStart);
    // printf("Set water fog start\n");
    water_shader.setFloat("fogLength", fogLength);
    // printf("Set water fog length\n");
    water_shader.setFloat("seaLevel", SEA_LEVEL);
    // printf("Set water sea level\n");
    water_shader.setVec3("lightPos", lightPos);
    // printf("Set water light position\n");
    water_shader.setBool("enableFog", enableFog);
    // printf("Set water enable fog\n");

    glDisable(GL_DEPTH_TEST);
    water_shader.setBool("near", true);
    glBindVertexArray(VAO_WATER);
    glDrawElements(GL_TRIANGLES, amount * 3, GL_UNSIGNED_INT, 0);
    glEnable(GL_DEPTH_TEST);

    // render occlusion mask
    // always render occlusion mask with regular terrain
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glBindFramebuffer(GL_FRAMEBUFFER, mask_fbo);
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
    occlusion_shader.setBool("enableFog", enableFog);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (y - 1) * (x - 1) * 2 * 3, GL_UNSIGNED_INT, 0);

    // render post-processing
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glBindVertexArray(screen_vao);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, window_width, window_height);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, terrain_tex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mask_tex);
    godray_shader.use();
    godray_shader.setUnsignedInt("screenWidth", window_width);
    godray_shader.setUnsignedInt("screenHeight", window_height);
    godray_shader.setMat4("model", glm::value_ptr(model));
    godray_shader.setMat4("view", glm::value_ptr(view));
    godray_shader.setMat4("projection", glm::value_ptr(projection));
    godray_shader.setVec4("clearColor", clearColor);
    godray_shader.setInt("prevTex", 0);
    godray_shader.setInt("maskTex", 1);
    godray_shader.setVec3("lightPos", lightPos);
    godray_shader.setFloat("fogStart", fogStart);
    godray_shader.setFloat("fogLength", fogLength);
    godray_shader.setVec3("cameraPos", camera.Position);
    godray_shader.setBool("enableFog", enableFog);
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
