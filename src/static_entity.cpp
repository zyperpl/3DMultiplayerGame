#include "entity.hpp"
#include "static_entity.hpp"


StaticEntity::StaticEntity(std::string model, std::string texture,
    vec3<float> position, vec3<float> rotation, vec3<float> scale, vec2<float> textureWrap) : Entity(model, texture, position, rotation, scale, textureWrap)
{

}

StaticEntity::StaticEntity(const SceneObject &obj) : Entity(obj)
{

}
