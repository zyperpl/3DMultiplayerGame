#include "shader.hpp"
#include "log.hpp"
#include "config.hpp"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <memory>

Shader::Shader(const char *name, GLenum type)
{
  this->name = name;
  this->type = type;

  this->compile();
}

void Shader::compile()
{
  if (this->id > 0 && bufferPtr) return;

  Log::i("Compiling %d shader '%s'...\n", type, name);

  // do not load if already loaded
  if (!bufferPtr)
  {
    // load file
    FILE *file = fopen((std::string(DATA_FOLDER)+name).c_str(), "rb");
    if (!file) {
      Log::w("Shader file %s cannot be opened!\n", name);
      return;
    }

    fseek(file, 0, SEEK_END);
    uint64_t fileSize = ftell(file);
    rewind(file);

    bufferPtr.reset(new char[fileSize+1]);
    fread(bufferPtr.get(), fileSize, sizeof(char), file);
    fclose(file);

    bufferPtr.get()[fileSize] = '\0';
  }

  // compile shader
  int infoLogLength = 0;
  this->id = glCreateShader(this->type);
  const char *buf = bufferPtr.get();
  glShaderSource(this->id, 1, &buf, NULL);
  glCompileShader(this->id);

  glGetShaderiv(this->id, GL_INFO_LOG_LENGTH, &infoLogLength);
  if (infoLogLength > 0)
  {
    Log::w("Error while compiling shader %s!\n", name);
    std::unique_ptr<char[]> bufPtr(new char[infoLogLength+1]);
    char *buf2 = bufPtr.get();
    glGetShaderInfoLog(this->id, infoLogLength, NULL, buf2);
    buf2[infoLogLength-1] = '\0';
    Log::i("\n%s\n", buf2);
  } else
  {
    Log::i("Shader %s compiled (id=%lu).\n", name, id);
  }
}

void Shader::free()
{
  glDeleteShader(this->id);
  this->id = 0;
}
