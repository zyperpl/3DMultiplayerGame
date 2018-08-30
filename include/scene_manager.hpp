#pragma once

#include <vector>
#include <map>
#include <memory>

#include "log.hpp"
#include "scene.hpp"

typedef bool (*TagExecutorFunc)(std::string, SceneObject*);

class SceneManager
{
  private:
    std::vector<std::unique_ptr<Scene>> scenes;
    std::map<std::string, TagExecutorFunc> tagExecutor;

    template <class T>
    vec3<T> readResourceVector3(std::string line);
    template <class T>
    vec2<T> readResourceVector2(std::string line);

    std::string readResourceString(std::string line);
    std::string getTagName(std::string line);

    static bool tagParseModel   (std::string line, SceneObject *obj);
    static bool tagParseTexture (std::string line, SceneObject *obj);
    static bool tagParsePosition(std::string line, SceneObject *obj);
    static bool tagParseRotation(std::string line, SceneObject *obj);
    static bool tagParseScale   (std::string line, SceneObject *obj);
    static bool tagParseColor   (std::string line, SceneObject *obj);
    static bool tagParsePower   (std::string line, SceneObject *obj);
  public:
    SceneManager();
    bool load(const char *sceneName);
    Scene *get(const char *sceneName);
};
