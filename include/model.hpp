#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "math.hpp"

#include "tiny_obj_loader.h"

class Entity;

class Model
{
  public:
    Model(std::string name);

    void generate();
    void draw();

    ~Model();

  private:
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::vector<GLuint> vertexBuffers;
    std::vector<GLuint> normalBuffers;
    std::vector<GLuint> uvBuffers;
    std::vector<GLuint> indexBuffers;

    std::string name;

    friend class Entity;
};
