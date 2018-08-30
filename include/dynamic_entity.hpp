#pragma once

#include "entity.hpp"
#include "scene.hpp"

class DynamicEntity : public Entity
{
  public:
    DynamicEntity(std::string model, std::string texture,
        vec3<float> position, vec3<float> rotation, vec3<float> scale);
    DynamicEntity(const SceneObject &obj);

    virtual void update(Game *game) = 0;
};
