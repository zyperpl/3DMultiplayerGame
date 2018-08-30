#include "view.hpp"
#include "math.hpp"
#include "config.hpp"
#include "log.hpp"
#include "shader_manager.hpp"
#include "input.hpp"

#include <glm/gtc/matrix_transform.hpp>

View::View()
{
  Log::i("Initializing view...\n");

  if (!glfwInit()) {
    Log::e("Cannot initialize GLFW!\n");
  }

  glfwSetErrorCallback(Log::glfwErrorCallback);

  //*
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
  // */
  

  // create window
  vec2<int> resolution = Config::getResolution();
  window = glfwCreateWindow(resolution.x, resolution.y, WINDOW_TITLE, NULL, NULL);

  if (!window)
  {
    Log::e("Cannot create window '%s' (%d;%d)\n", WINDOW_TITLE, resolution.x, resolution.y);
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(0);
  glfwSetInputMode(window, GLFW_STICKY_KEYS, true);

  // init OpenGL 
  glewExperimental = true;
  if (glewInit() != GLEW_OK) {
    Log::e("GLEW initialization error!\n");
  }
  glGetError(); // reset error from glewInit

  glEnable              (GL_DEBUG_OUTPUT);
  glDebugMessageCallback((GLDEBUGPROC) Log::glMessageCallback, 0);
  
  GLuint vertexArrayID;
  glGenVertexArrays(1, &vertexArrayID);
  glBindVertexArray(vertexArrayID);

  
  //*
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glDepthMask(GL_TRUE);
  // */
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT); 
  glFrontFace(GL_CW);
  // */
  
  programID = ShaderManager::getProgram(2,
      "shaders/texture_fragment.glsl", GL_FRAGMENT_SHADER,
      "shaders/texture_vertex.glsl", GL_VERTEX_SHADER);
  
  // post-processing framebuffer
  framebuffers.emplace_back(createTextureFramebuffer(resolution.x, resolution.y) );

  // shadow map framebuffer
  vec2<int> shadowResolution = Config::getShadowResolution();
  framebuffers.emplace_back(createDepthFramebuffer(shadowResolution.x, shadowResolution.y) );
 
  framebuffers[0]->programID = ShaderManager::getProgram(2, 
      "shaders/framebuffer_fragment.glsl", GL_FRAGMENT_SHADER,
      "shaders/passthrough_vertex.glsl", GL_VERTEX_SHADER);

  framebuffers[1]->programID = ShaderManager::getProgram(2, 
      "shaders/shadowmap_fragment.glsl", GL_FRAGMENT_SHADER,
      "shaders/mvp_vertex.glsl", GL_VERTEX_SHADER);

  Log::i("Shader programs: %d %d %d\n", framebuffers[0]->programID, framebuffers[1]->programID, programID);

  static const GLfloat quad_vertex_data[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    1.0f,  1.0f, 0.0f,
  };
  glGenBuffers(1, &fbVertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, fbVertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertex_data), quad_vertex_data, GL_STATIC_DRAW);

}

View::~View()
{
  if (window) glfwDestroyWindow(window);
}

bool View::isOpen()
{
  if (window == NULL) return false;

  return !glfwWindowShouldClose(window);
}

void View::clear()
{
  glClearColor(0.9, 0.9, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void View::enableFramebuffer(uint32_t n)
{
  if (!framebuffers.empty()) 
  {
    auto *fb = framebuffers.at(n).get();

    glBindFramebuffer(GL_FRAMEBUFFER, fb->ID);
    glViewport(0, 0, fb->width, fb->height);
  } 
}

void View::disableFramebuffer()
{
  // enable default framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  //auto resolution = Config::getResolution();
  //double ratio = static_cast<double>(resolution.y) / static_cast<double>(resolution.x);
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
}

void View::renderFramebuffer(uint32_t n)
{
  if (framebuffers.empty()) 
  {
    Log::w("No framebuffers available to be rendered!\n");
    return;
  }
  auto *targetFb = framebuffers.at(0).get();
  auto *sourceFb = framebuffers.at(n).get();

  GLuint fbTextureProgramID = targetFb->programID;
  glUseProgram(fbTextureProgramID);
  
  int samplerID = 0;
  glActiveTexture(GL_TEXTURE0 + samplerID);
  glBindTexture(GL_TEXTURE_2D, sourceFb->textureID);

  GLuint textureSamplerID = glGetUniformLocation(fbTextureProgramID, "textureSampler");
  glUniform1i(textureSamplerID, samplerID);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, fbVertexBuffer);
  glVertexAttribPointer(
      0,
      3,
      GL_FLOAT,
      GL_FALSE,
      0,
      NULL
  );
  
  glDrawArrays(GL_TRIANGLES, 0, 6);

  glDisableVertexAttribArray(0);
}

void View::swapWindowBuffers()
{
  static GLuint err;
  GLuint error = glGetError();
  if (error != 0)
  {
    // print error only once
    if (err != error)
    {
      Log::w("OpenGL error: %d!\n", error);
    }
    err = error;
  }

  glfwSwapBuffers(window);
}

Framebuffer *View::createTextureFramebuffer(int width, int height)
{
  Framebuffer *fb = new Framebuffer;
  fb->width   = width;
  fb->height  = height;

  // generate framebuffer
  glGenFramebuffers(1, &fb->ID);
  glBindFramebuffer(GL_FRAMEBUFFER, fb->ID);

  // generate texture
  glGenTextures(1, &fb->textureID);
  glBindTexture(GL_TEXTURE_2D, fb->textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, fb->textureID, 0);
  GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
  glDrawBuffers(1, drawBuffers);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  // generate depth buffer
  GLuint depthBuffer;
  glGenRenderbuffers(1, &depthBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    Log::w("Framebuffer (size=%d;%d) cannot be created!\n", width, height);
    return nullptr;
  }

  return fb;
}


Framebuffer *View::createDepthFramebuffer(int width, int height)
{
  Framebuffer *fb = new Framebuffer;
  fb->width   = width;
  fb->height  = height;

  // generate framebuffer
  glGenFramebuffers(1, &fb->ID);
  glBindFramebuffer(GL_FRAMEBUFFER, fb->ID);

  // generate texture
  glGenTextures(1, &fb->textureID);
  glBindTexture(GL_TEXTURE_2D, fb->textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);

  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
  float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
  
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, fb->textureID, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    Log::w("Framebuffer (size=%d;%d) cannot be created!\n", width, height);
    return nullptr;
  }

  return fb;
}
