#pragma once

#include <cstdio> 
#include <cstring>
#include <cstdlib>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <memory.h>
#include <assert.h>

#include "game.hpp"

#define EPOLL_SIZE    256
#define EPOLL_TIMEOUT UPDATE_INTERVAL_MS

#define BUFFER_SIZE   512

#define DUMPARRAY(a, s) for(size_t i=0;i<s;i++){printf("%x ", a[i]);}puts("");

class Game;

enum class NetworkPackageType
{
  PlayerInfo = 0b10000001,
  PlayerShot = 0x10000010,
  NewPlayer  = 0x01000001
};

struct __attribute__ ((packed)) NetworkPackage 
{
  NetworkPackageType type;
  uint64_t tick;
  size_t length;
};

struct NetworkPackageContainer
{
  NetworkPackage package;
  char *data;
};

class Server
{
  public:
    Server(int port);
    ~Server();
    
    /*
     * Process network events and update game with new data.
     */
    void process(Game *game);

    /*
     * Send updated game data to clients
     */
    void send(Game *game);

    /*
     * Send updated game data to specified client 
     */
    void send(Player *player, Game *game);

    int inline getSocket() { return sfd; }
    int inline getEpoll()  { return efd; }

  private:
    /*
     * Socket file descriptor.
     */
    int sfd = -1;

    /*
     * Epoll file descriptor.
     */
    int efd = -1;

    inline uint64_t sockaddrID(struct sockaddr_in *sockaddr) {
      uint64_t v;
      v = sockaddr->sin_addr.s_addr << 4;
      v += sockaddr->sin_port;
      return v;
    }
};
