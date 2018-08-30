#pragma once

#include <string>
#include <cstdio>

#include "math.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Entity;

class Texture
{
  public:
    Texture(std::string name, vec2<float> textureWrap = { 1, 1 });
    void bind(GLuint samplerLocation, GLuint wrapLocation);
    void generate();
    ~Texture();
  private:
    std::string name;
    uint8_t *data{NULL};

    int width;
    int height;
    int components;

    vec2<float> textureWrap;

    GLuint id{0};
    GLuint samplerID = 0;

    friend class Entity;
};
