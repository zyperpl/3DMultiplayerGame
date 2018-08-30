#include <fstream>
#include <sstream>
#include <string.h>

#include "scene_manager.hpp"
#include "log.hpp"

#define ADD_TAG_FUNC(name, func) \
  this->tagExecutor.insert(std::pair<std::string,TagExecutorFunc>(name, func))

SceneManager::SceneManager() 
{ 
  //this->tagExecutor.insert(std::pair<std::string, TagExecutorFunc>("mdl", tagParseModel) );
  ADD_TAG_FUNC("mdl", tagParseModel);
  ADD_TAG_FUNC("tex", tagParseTexture);
  ADD_TAG_FUNC("pos", tagParsePosition);
  ADD_TAG_FUNC("rot", tagParseRotation);
  ADD_TAG_FUNC("scl", tagParseScale);
  ADD_TAG_FUNC("col", tagParseColor);
  ADD_TAG_FUNC("pwr", tagParsePower);
}

template <class T>
vec3<T> SceneManager::readResourceVector3(std::string line)
{
  std::istringstream iss(line);

  std::string type;
  T x, y, z;
  if (!(iss >> type >> x >> y >> z)) {
    Log::w("Scene parsing error! Cannot read resource %s vector.\n", type);
  }

  return { x, y, z };
}

template <class T>
vec2<T> SceneManager::readResourceVector2(std::string line)
{
  std::istringstream iss(line);

  std::string type;
  T x, y;
  if (!(iss >> type >> x >> y)) {
    Log::w("Scene parsing error! Cannot read resource vector2.\n");
  }

  return { x, y };
}

std::string SceneManager::readResourceString(std::string line)
{
  std::istringstream iss(line);

  std::string type, val;
  if (!(iss >> type >> val)) {
    Log::w("Scene parsing error! Cannot read resource string value!\n");
  }

  return val;
}

bool SceneManager::load(const char *sceneName)
{
  // check if already loaded
  if (get(sceneName)) return true;

  std::ifstream file(sceneName);
  if (!file.is_open()) 
  {
    Log::e("Cannot load scene file '%s'!\n", sceneName);
    return false;
  }

  Scene *scene = new Scene(sceneName);

  std::string line;
  while (std::getline(file, line))
  {
    if (line == "" || line.at(0) == '#') continue;

    /*
    SceneObject *obj = new SceneObject();

    //next line
    std::getline(file, line);

    while (line != "")
    {
      if (strncmp(line.c_str(), "mdl", 3) == 0) {
        obj->model = readResourceString(line);
      } else
      if (strncmp(line.c_str(), "pos", 3) == 0) {
        obj->position = readResourceVector3<float>(line);
      } else
      if (strncmp(line.c_str(), "rot", 3) == 0) {
        obj->rotation = readResourceVector3<float>(line);
      } else
      if (strncmp(line.c_str(), "scl", 3) == 0) {
        obj->scale    = readResourceVector3<float>(line);
      } else
      if (strncmp(line.c_str(), "tex", 3) == 0) {
        obj->texture = readResourceString(line);
        obj->textureWrap = readResourceVector2<float>(line.erase(0, 4));
      } 
      if (!std::getline(file, line)) line = "";
    }

    scene->addSceneObject(obj);
    */

    SceneObject *obj = nullptr;

    if (line != "") {
      obj = new SceneObject();
      obj->attributes.type.fromName(line);

      while (line != "") 
      {
        if (!std::getline(file, line)) 
        {
          line = "";
          continue;
        }
        if (line == "") continue;
        if (line.size() > 0 && line.at(0) == '#') continue;

        std::string tagName = getTagName(line);

        try {
          bool ret = tagExecutor.at(tagName)(line, obj);
          if (!ret) {
            Log::w("Tag %s not executed correctly!\n", tagName.c_str());
          } 
        }

        catch (std::out_of_range &e) {
          Log::e("Undefined tag \"%s\" when parsing \"%s\"!\n", 
              tagName.c_str(), line.c_str());
          delete obj;
          obj = nullptr;
        }

      }


    }

    if (obj != nullptr) {
      Log::i("SceneObject: \"%s\"\n", obj->attributes.type.getName().c_str());
      scene->addSceneObject(obj);
    }
  }

  this->scenes.emplace_back(std::move(scene) );

  return true;
}

bool SceneManager::tagParseModel(std::string line, SceneObject *obj)
{
  std::istringstream iss(line);
  std::string tag, model;

  if (!(iss >> tag >> model)) return false;

  obj->model = model;

  return true;
}
bool SceneManager::tagParseTexture(std::string line, SceneObject *obj)
{
  std::istringstream iss(line);
  std::string tag, texture;

  if (!(iss >> tag >> texture)) return false;

  float wrapU = 1.0, wrapV = 1.0;
  iss >> wrapU >> wrapV;

  obj->texture = texture;
  obj->textureWrap.x = wrapU;
  obj->textureWrap.y = wrapV;

  return true;
}
bool SceneManager::tagParsePosition(std::string line, SceneObject *obj)
{
  std::istringstream iss(line);
  std::string tag;
  float x,y,z;

  if (!(iss >> tag >> x >> y >> z)) return false;

  obj->position.x = x;
  obj->position.y = y;
  obj->position.z = z;

  return true;
}
bool SceneManager::tagParseRotation(std::string line, SceneObject *obj)
{
  std::istringstream iss(line);
  std::string tag;
  float x,y,z;

  if (!(iss >> tag >> x >> y >> z)) return false;

  obj->attributes.rotation.x = x;
  obj->attributes.rotation.y = y;
  obj->attributes.rotation.z = z;

  return true;
}
bool SceneManager::tagParseScale(std::string line, SceneObject *obj)
{
  std::istringstream iss(line);
  std::string tag;
  float x,y,z;

  if (!(iss >> tag >> x >> y >> z)) return false;

  obj->attributes.scale.x = x;
  obj->attributes.scale.y = y;
  obj->attributes.scale.z = z;

  return true;
}

bool SceneManager::tagParseColor(std::string line, SceneObject *obj)
{
  std::istringstream iss(line);
  std::string tag;
  float x,y,z;

  if (!(iss >> tag >> x >> y >> z)) return false;

  obj->attributes.color.x = x;
  obj->attributes.color.y = y;
  obj->attributes.color.z = z;

  return true;
}

bool SceneManager::tagParsePower(std::string line, SceneObject *obj)
{
  std::istringstream iss(line);
  std::string tag;
  float x;

  if (!(iss >> tag >> x)) return false;

  obj->attributes.power = x;

  return true;
}

std::string SceneManager::getTagName(std::string line)
{
  if (line.size() < 3) return "";
  return line.substr(0, 3);
}

Scene *SceneManager::get(const char *sceneName)
{
  Scene *scene = NULL;
  for (int i = scenes.size()-1; i >= 0; --i)
  {
    if (scenes.at(i).get()->name == sceneName)
    {
      scene = scenes.at(i).get();        
    }

  }

  return scene;
}
