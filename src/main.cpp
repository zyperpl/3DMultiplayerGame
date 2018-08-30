#include "scene.hpp"
#include "scene_manager.hpp"
#include "game.hpp"

#include "config.hpp"

#include <thread>

void loadScenes(SceneManager &manager)
{
  // preloaded scenes list
  manager.load(INIT_SCENE);
}

int main(void)
{
  printf("Starting...\n");

  SceneManager sceneManager;
  loadScenes(sceneManager);
  Scene *scene = sceneManager.get(INIT_SCENE);

  if (!scene) {
    Log::e("Cannot loaded INIT_SCENE %s!\n", INIT_SCENE);
    exit(1);
  }

  Game game;

  game.load(scene);
  if (!game.init())
  {
    Log::e("Cannot initialize game!\n");
    exit(2);
  }

  game.joinThreads();

  printf("Starting main loop...\n");
  game.run();

  game.joinThreads();
  printf("Ending..\n");
  return 0;
}

