#ifndef __VERTEX__
#define __VERTEX__

#include <glm/glm.hpp>

#pragma once
#pragma pack(push, 1)
struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
};
#pragma pack(pop)
static_assert(sizeof(Vertex) == 6 * sizeof(float), "Vertex struct has unexpected padding");

#endif