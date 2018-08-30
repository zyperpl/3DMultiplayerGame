#include "math.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

std::vector<unsigned int> indexVBO(float vertices[], size_t size)
{
  std::vector<unsigned int> indices;
  unsigned int i, j, a;
  a = 0;
  bool found = false;
  for (i = 0; i < size; i+=3)
  {
    found = false;
    for (j = 0; j < indices.size(); j+=1)
    {
      if (&vertices[i] == &vertices[j*3])
      {
        continue;
      }
      if (vertices[i+0] == vertices[j*3+0] 
      &&  vertices[i+1] == vertices[j*3+1]
      &&  vertices[i+2] == vertices[j*3+2])
      {
        indices.push_back(j);
        found = true;
      }
    }
    if (!found) 
    {
      indices.push_back(a);
    }
    a++;
  }
  //std::reverse(indices.begin(), indices.end() );
  return indices;
}
