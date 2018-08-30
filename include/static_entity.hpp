#pragma once

#include <string>
#include "entity.hpp"
#include "math.hpp"
#include "scene.hpp"

class StaticEntity : public Entity
{
  public:
    StaticEntity(std::string model, std::string texture,
        vec3<float> position, vec3<float> rotation, vec3<float> scale, vec2<float> textureWrap);

    StaticEntity(const SceneObject &obj);
};
