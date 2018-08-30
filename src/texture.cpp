#include "texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "log.hpp"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>

#include "config.hpp"

Texture::Texture(std::string name, vec2<float> textureWrap) : 
  name(name), 
  textureWrap(textureWrap)
{
  FILE *file = fopen((DATA_FOLDER+name).c_str(), "rb");
  if (file) {
    this->data = stbi_load_from_file(file, &this->width, &this->height, &this->components, 0);
    fclose(file);
  } else
  {
    Log::w("Texture file '%s' cannot be opened!\n", name.c_str());
  }
}

void Texture::generate()
{
  Log::i("Generating texture %s buffers...\n", name.c_str());

  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  if (this->components == 3)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, &data[0]);
  } else
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
  }

  free(data);
  data = NULL;

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::bind(GLuint samplerLocation, GLuint wrapLocation)
{
  glActiveTexture(GL_TEXTURE0 + this->samplerID);
  glBindTexture(GL_TEXTURE_2D, id);

  glUniform1i(samplerLocation, this->samplerID);

  glUniform2f(wrapLocation, this->textureWrap.x, this->textureWrap.y);
}

Texture::~Texture()
{
  if (data) delete[] data;
}
