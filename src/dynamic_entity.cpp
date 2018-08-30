#include "dynamic_entity.hpp"
#include "view.hpp"

DynamicEntity::DynamicEntity(
    std::string model, std::string texture,
    vec3<float> position, vec3<float> rotation, vec3<float> scale) 
  : Entity(model, texture, position, rotation, scale)
{
}

DynamicEntity::DynamicEntity(const SceneObject &obj) : Entity(obj)
{
  printf("DynamicEntity creation: %f;%f;%f\n", position.x, position.y, position.z);
}
