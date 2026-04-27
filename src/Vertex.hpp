#ifndef __VERTEX__
#define __VERTEX__

#pragma once
#pragma pack(push, 1)
struct Vertex {
  float position[3];
  float normal[3];
};
#pragma pack(pop)
static_assert(sizeof(Vertex) == 6 * sizeof(float), "Vertex struct has unexpected padding");

#endif