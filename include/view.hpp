#pragma once

#include <cstdio>
#include <cstdlib>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

class Game;
class Input;

#include "math.hpp"
#include "shader_manager.hpp"

#define VIEW_UPDATE_INTERVAL_MS 16

struct Framebuffer
{
  GLuint ID;
  GLuint textureID;
  int width;
  int height;

  GLuint programID;
};

class View
{
  public:
    View();
    ~View();
    bool isOpen();

    void clear();
    void swapWindowBuffers();
    void enableFramebuffer(uint32_t n = 0);
    void renderFramebuffer(uint32_t n = 0);
    void disableFramebuffer();
  private:
    GLFWwindow *window;
    GLuint programID;
    GLuint fbVertexBuffer;
    GLuint vertexbuffer;

    vec3<float> *cameraPosition{nullptr};

    Framebuffer *createTextureFramebuffer(int width, int height);
    Framebuffer *createDepthFramebuffer(int width, int height);
    std::vector<std::unique_ptr<Framebuffer>> framebuffers;

    friend class Game;
};
