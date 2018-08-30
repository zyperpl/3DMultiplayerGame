#include "config.hpp"

const vec2<int> Config::getResolution() 
{
  return { 1920, 1080 };
}

const vec2<int> Config::getShadowResolution()
{
  return { 1024*5, 1024*5 };
}

int Config::key(Keys key)
{
  return key;
}

// TODO: replace hardcoded variables
const ServerInfo Config::getServerInfo()
{
  return { "127.0.0.1", 1337 };
  return { "zyper.pl" , 1337 };
}
