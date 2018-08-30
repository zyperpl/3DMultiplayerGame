#pragma once

#define INIT_SCENE "data/levels/init.scene"
#define DATA_FOLDER "data/"

#define WINDOW_TITLE "Tanki: 3D Multiplayer Shooter"

#define UPDATE_INTERVAL_MS 30

#define NETWORK_UPDATE_INTERVAL_MS UPDATE_INTERVAL_MS
#define NETWORK_BUFFER_SIZE 512

#include <GLFW/glfw3.h>
#include "math.hpp"

enum Keys
{
  Exit = GLFW_KEY_ESCAPE, 
  Left = GLFW_KEY_A,
  Right = GLFW_KEY_D, 
  Up = GLFW_KEY_W,
  Down = GLFW_KEY_S,
  Action = GLFW_KEY_SPACE, 
  KEYS_MAX
};

struct ServerInfo
{
  const char *ip;
  int port;
};

class Config
{
  public:
    static const vec2<int> getResolution();
    static const vec2<int> getShadowResolution();
    static const ServerInfo getServerInfo();
    static uint64_t getNetworkBufferSize();

    static int key(Keys key);
};
