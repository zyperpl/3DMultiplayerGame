#pragma once

#include <vector>

template <class T>
struct vec2
{
  T x;
  T y;
};

#define VEC2(v) v.x,v.y

template <class T>
struct vec3
{
  union {
    struct {
      T x;
      T y;
      T z;
    };
    struct {
      T r;
      T g;
      T b;
    };
  };
};

#define VEC3(v) v.x,v.y,v.z

#define VEC3F_STR "%.2f;%.2f;%.2f"
#define VEC3F_STR "%.2f;%.2f;%.2f"

std::vector<unsigned int> indexVBO(float vertices[], size_t size);
