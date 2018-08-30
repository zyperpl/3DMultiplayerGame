#include "scene.hpp"

std::map<std::string, int> SceneObjectType::NAMES = {
  { "static", 0 }, { "player", 1 }, { "camera", 2 }, { "light", 3 }
};

bool SceneObjectType::operator==(const char *t) const
{
  auto myName = this->getName();
  return strncmp(myName.c_str(), t, myName.size()) == 0;
}

int SceneObjectType::fromName(std::string name)
{
  this->number = NAMES.at(name); 
  return this->number;
}

std::string SceneObjectType::getName() const
{
  int num = this->number;
  return (*std::find_if(std::begin(NAMES), std::end(NAMES),
      [num](const std::pair<std::string,int> pair) {
        return pair.second == num;
      })).first;
}

Scene::Scene(const char *name)
{
  this->name = name;
}

Scene::~Scene()
{
  for (const SceneObject *obj : objects) delete obj;
}

std::vector<SceneObject *> Scene::getSceneObjects()
{
  return this->objects;
}

std::string Scene::getName()
{
  return this->name;
}

void Scene::addSceneObject(SceneObject *obj)
{
  this->objects.push_back(obj);
}

