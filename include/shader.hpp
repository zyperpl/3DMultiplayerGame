#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Shader
{
  public:
    Shader(const char *name, GLenum type);

    void compile();
    void free();
  private:
    const char *name;
    GLenum type;

    std::unique_ptr<char[]> bufferPtr;
    GLuint id{0};

    friend class ShaderManager;
};
