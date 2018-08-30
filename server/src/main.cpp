#include <cstdio> 
#include <cstring>
#include <cstdlib>
#include <chrono>

#include "network.hpp"
#include "game.hpp"

#define ERROR_EXIT 1
#define GOOD_EXIT  0

#define BUFFER_LENGTH 1024

void printHelp(const char *executableName)
{
  fprintf(stderr, "Usage:\n%s <port>\n", executableName);
}

/*
 * Returns time in milliseconds.
 */
uint64_t getTime()
{
  return std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::system_clock::now().time_since_epoch()
  ).count();
}

int main(int argc, char *argv[])
{
  if (argc <= 1 || argc > 2)
  {
    printHelp(argv[0]);
    return ERROR_EXIT;
  }
  int port = atoi(argv[1]);
  
  Server server(port);

  if (server.getSocket() == -1 || server.getEpoll() == -1)
  {
    fprintf(stderr, "Server creation error!\n");
    return ERROR_EXIT;
  }

  Game *game = new Game();

  auto ms = getTime();
  bool running = true;
  
  while (running)
  {
    server.process(game);
    
    auto currentTimeMs = getTime();
    if (currentTimeMs - ms >= UPDATE_INTERVAL_MS)
    {
      ms = currentTimeMs;
      game->step();
      server.send(game);
    }
  }

  delete game;
  
  return GOOD_EXIT;
}
