#include "entity.hpp"
#include "texture.hpp"
#include "model.hpp"
#include "scene.hpp"

#include "shader_manager.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "log.hpp"

Entity::Entity() {}
 
Entity::Entity(std::string model, std::string texture, vec2<float> textureWrap) {
  this->models.push_back(new Model(model));
  this->textures.push_back(new Texture(texture, textureWrap));
}

Entity::Entity(std::string model, std::string texture, vec3<float> position, vec3<float> rotation, vec3<float> scale, vec2<float> textureWrap)
  : Entity(model, texture, textureWrap)
{
  object.position = position;
  object.attributes.rotation = rotation;
  object.attributes.scale = scale;
}

Entity::Entity(const SceneObject &obj)
  : models{new Model(obj.model)}, textures{new Texture(obj.texture, obj.textureWrap)} 
{
  object = obj;
}

glm::mat4 Entity::getModelMatrix() const
{
  glm::mat4 translationMatrix = glm::translate(glm::mat4(1), glm::vec3(position.x, position.y, position.z) );
  glm::mat4 rotationMatrix = glm::toMat4(glm::quat(glm::vec3(rotation.x, rotation.y, rotation.z)) );
  glm::mat4 scaleMatrix = glm::scale(glm::vec3(scale.x, scale.y, scale.z) );

  return translationMatrix * rotationMatrix * scaleMatrix;
}

void Entity::draw(glm::mat4 projection, glm::mat4 view, std::vector<Light*> lights) const
{
  GLint programID;
  glGetIntegerv(GL_CURRENT_PROGRAM, &programID);

  auto modelMatrix = getModelMatrix();

  GLint mID = glGetUniformLocation(programID, "M");
  if (mID != -1) glUniformMatrix4fv(mID, 1, GL_FALSE, &modelMatrix[0][0]);

  GLint pID = glGetUniformLocation(programID, "P");
  if (pID != -1) glUniformMatrix4fv(pID, 1, GL_FALSE, &projection[0][0]);
  
  GLint vID = glGetUniformLocation(programID, "V");
  if (vID != -1) glUniformMatrix4fv(vID, 1, GL_FALSE, &view[0][0]);

  for (size_t i = 0; i < lights.size(); i++)
  {
    if (i >= 10) {
      Log::e("Too many lights! %lu >= 10\n", lights.size());
    }
    char uniformPositionName[] = "world_light[0].position";
    uniformPositionName[12] += i; // sets proper character as index 
    //TODO: make proper char replacement

    GLint lightPositionID = glGetUniformLocation(programID, uniformPositionName);
    if (lightPositionID == -1) break; // skip if shader does not support lights
    glUniform3fv(lightPositionID, 1, &lights.at(i)->position.x);

    char uniformColorName[] = "world_light[0].color";
    uniformColorName[12] += i; 

    GLint lightColorID = glGetUniformLocation(programID, uniformColorName);
    glUniform3fv(lightColorID, 1, &lights.at(i)->color.r);
    
    char uniformPowerName[] = "world_light[0].power";
    uniformPowerName[12] += i; 

    GLint lightPowerID = glGetUniformLocation(programID, uniformPowerName);
    glUniform1f(lightPowerID, lights.at(i)->power);
  }

  GLint textureSamplerLocation = glGetUniformLocation(programID, "textureSampler");
  GLint textureWrapLocation    = glGetUniformLocation(programID, "textureWrap");
  if (textureSamplerLocation != -1 && textureWrapLocation != -1)
  {
    for (size_t i = 0; i < this->textures.size(); i++)
    {
      this->textures.at(i)->bind(textureSamplerLocation, textureWrapLocation);
    }
  }

  for (size_t i = 0; i < this->models.size(); i++)
  {
    this->models.at(i)->draw();
  }

}

void Entity::generate()
{
  Log::i("Generating models and textures for %s..\n", this->object.attributes.type.getName().c_str());

  for (const auto &model : models) model->generate();
  for (const auto &texture : textures) texture->generate();

  this->generated = true;
}

Entity::~Entity() {
  for (size_t i = 0; i < this->models.size(); i++)
  {
    delete this->models.at(i);
  }

  for (size_t i = 0; i < this->textures.size(); i++)
  {
    delete this->textures.at(i);
  }
}
