#pragma once

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>

#include "math.hpp"
#include "scene.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Game;
class Model;
class Texture;

struct Light;

class Entity
{
public:
  Entity();
  Entity(std::string model, std::string texture, vec2<float> textureWrap = { 1, 1 });
  Entity(
      std::string model, 
      std::string texture, 
      vec3<float> position, 
      vec3<float> rotation, 
      vec3<float> scale, 
      vec2<float> textureWrap = { 1, 1 }
  );
  Entity(const SceneObject &obj);
  virtual ~Entity();

  void generate();
  void draw(glm::mat4 projection, glm::mat4 view, std::vector<Light*> lights) const;

  inline vec3<float> getPosition() { return object.position; }
  inline vec3<float> getRotation() { return object.attributes.rotation; }
  inline vec3<float> getScale()    { return object.attributes.scale; }

  inline void setPosition(vec3<float> v) { this->position = v; }
  inline void setRotation(vec3<float> v) { this->rotation = v; }
  inline void setScale(vec3<float> v) { this->scale = v; }
protected:
  std::vector<Model*> models;
  std::vector<Texture*> textures;

  /*
   *  Returns true if textures and models where generated.
   */
  inline bool isGenerated() { return this->generated; }
  
  vec3<float> &position = object.attributes.position;
  vec3<float> &rotation = object.attributes.rotation;
  vec3<float> &scale    = object.attributes.scale;

  glm::mat4 getModelMatrix() const;

private:
  SceneObject object;
  bool generated{false};
  friend class Game;
};
