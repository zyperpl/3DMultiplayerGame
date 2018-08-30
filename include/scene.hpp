#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "log.hpp"
#include "math.hpp"

class SceneManager;

struct SceneObjectType
{
  int number;

  bool operator==(const char *t) const;

  int fromName(std::string name);
  std::string getName() const;

  static std::map<std::string, int> NAMES;
};
  
struct SceneObjectData
{
  SceneObjectType type{0};

  SceneObjectData() {};

  vec3<float> position{{{0.0,0.0,0.0}}};

  union
  {
    struct {
      vec3<float> rotation{{{0.0,0.0,0.0}}};
      vec3<float> scale{{{0.0, 0.0, 0.0}}};
    };
    struct {
      vec3<float> color;
      float power;
    };
    vec2<float> fov;
  };
};

struct SceneObject
{
  SceneObject() {}

  SceneObject(vec3<float> position, vec3<float> rotation, vec3<float> scale)
  {
    this->attributes.position = position;
    this->attributes.rotation = rotation;
    this->attributes.scale = scale;
  }

  SceneObject(const SceneObject &obj)
  {
    this->attributes = obj.attributes;
  }

  const SceneObject &operator=(const SceneObject &obj)
  {
    this->model = obj.model;
    this->attributes = obj.attributes;
    this->texture = obj.texture;
    this->textureWrap = obj.textureWrap;
    return obj;
  }

  SceneObjectData attributes;

  std::string model{""};

  vec3<float> &position = attributes.position;

  std::string texture{""};
  vec2<float> textureWrap{0.0,0.0};
};

struct Light
{
  vec3<float> position;
  vec3<float> color;
  float power{100.0};

  Light(const SceneObject &obj) 
    : position{obj.position}, color{obj.attributes.color}, power{obj.attributes.power} {}

  Light(vec3<float> position, vec3<float> color) 
    : position{position}, color{color} {}
};

class Scene
{
  public:
    Scene(const char *name);
    ~Scene();
    std::vector<SceneObject *> getSceneObjects();
    std::string getName();

    void addSceneObject(SceneObject *obj);

  private:
    std::vector<SceneObject *> objects;
    std::string name;


    friend class SceneManager;
};
