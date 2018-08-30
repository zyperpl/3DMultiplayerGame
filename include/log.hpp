#pragma once

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <string.h>

#include <stdexcept>

#define VEC3F_STR "%.2f;%.2f;%.2f"

#define LOG_MSG_BUFFER 1024

#include <execinfo.h>

#define BACKTRACE() {\
  void *buffer[100];\
  auto nptrs = backtrace(buffer, 100);\
  char **strings = backtrace_symbols(buffer, nptrs);\
  for (int i = 0; i < nptrs; i++) printf("  %s\n", strings[i]);\
  puts("");\
  fflush(stdout);\
  free(strings);\
}

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Log
{
  public:
    /*
     * Prints normal information.
     */
    static int i(const char *fmt, ...);

    /*
     * Prints warning.
     */
    static int w(const char *fmt, ...);

    /*
     * Prints text and throws exception.
     */
    static int e(const char *fmt, ...);


    static void glMessageCallback( GLenum source,
                      GLenum type,
                      GLuint id,
                      GLenum severity,
                      GLsizei length,
                      const GLchar* message,
                      const void* userParam);

    static void glfwErrorCallback(int error, const char *description);

  private:
    static int print(char *str, const char *fmt, va_list args);

};
