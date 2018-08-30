#include "log.hpp"

int Log::w(const char *fmt, ...)
{
  char *buffer = new char[LOG_MSG_BUFFER]; 
  memset(buffer, 0, LOG_MSG_BUFFER);

  va_list args;
  va_start(args, fmt);
  int ret = Log::print(buffer, fmt, args);

  char *buffer2 = new char[LOG_MSG_BUFFER];
  memset(buffer2, 0, LOG_MSG_BUFFER);
  ret += sprintf(buffer2, "[Warning] %s", buffer);

  va_end(args);

  // TODO: add generalization here
  fprintf(stderr, "%s\n", buffer2);

  delete[] buffer;
  delete[] buffer2;

  return ret;
}

int Log::print(char *str, const char *fmt, va_list args)
{
  return vsprintf(str, fmt, args);
}

int Log::e(const char *fmt, ...)
{
  BACKTRACE();

  char *buffer = new char[LOG_MSG_BUFFER]; 
  memset(buffer, 0, LOG_MSG_BUFFER);

  va_list args;
  va_start(args, fmt);
  int ret = Log::print(buffer, fmt, args);

  char *buffer2 = new char[LOG_MSG_BUFFER];
  memset(buffer2, 0, LOG_MSG_BUFFER);
  ret += sprintf(buffer2, "⛔ ERROR ⛔ %s", buffer);
  
  va_end(args);

  fprintf(stderr, "%s", buffer2);

  printf("\nFATAL RUNTIME ERROR\n");
  fflush(stdout);
  throw std::runtime_error(buffer);
 
  delete[] buffer;
  delete[] buffer2;

  return ret;
}

int Log::i(const char *fmt, ...)
{
  char *buffer = new char[LOG_MSG_BUFFER]; 
  memset(buffer, 0, LOG_MSG_BUFFER);

  va_list args;
  va_start(args, fmt);
  int ret = Log::print(buffer, fmt, args);

  char *buffer2 = new char[LOG_MSG_BUFFER];
  memset(buffer2, 0, LOG_MSG_BUFFER);
  ret += sprintf(buffer2, " %s", buffer);

  va_end(args);

  // TODO: add generalization here
  printf("%s", buffer2);

  delete[] buffer;
  delete[] buffer2;

  return ret;
}

void Log::glMessageCallback( GLenum source,
                      GLenum type,
                      GLuint id,
                      GLenum severity,
                      GLsizei length,
                      const GLchar* message,
                      const void* userParam )
{
  (void)(userParam);

  fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message);
  
  if (type == GL_DEBUG_TYPE_ERROR) 
  {
    Log::e("OpenGL error!");
  }
}


void Log::glfwErrorCallback(int error, const char *description)
{
  fprintf(stderr, "GLFW_ERROR %d: %s!\n", error, description);
  throw std::runtime_error("GLFW ERROR");
}
