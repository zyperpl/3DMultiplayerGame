#pragma once

#include <cstdio> 
#include <cstring> 
#include <cstdlib> 
#include <mutex>
#include <queue>
#include <vector>
#include <map>
#include <algorithm>
#include <memory>

#include <arpa/inet.h>
#include <sys/socket.h>

#include <string>
#include <math.hpp>

#include "log.hpp"

class Game;
class Entity;

enum class NetworkPackageType
{
  PlayerInfo = 0b10000001,
  PlayerShot = 0x10000010,
  NewPlayer  = 0x01000001
};

union NetworkPacketData
{
  struct {
    vec3<float> position; vec3<float> rotation; vec3<double> velocity;
  };
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

class Network
{
  public:
    /*
     *  Creates socket and saves connection to @serverName at @port.
     */
    Network(const char *serverName, int port, const uint64_t &tick);
    
    ~Network();

    /*
     *  Sends @data of size @size to socket.
     */
    size_t send(void *data, size_t size);

    /*
     *  Sends text string @str to socket.
     */
    size_t send(std::string str);

    /*
     *  Starts continuous communication with server.
     */
    void beginCommunication(Game *game);

    /*
     *  Adds packet to network outcoming packet queue
     */
    bool addPacket(NetworkPackageType type, NetworkPacketData data);

  protected:
    std::mutex packageQueueMutex;

    /*
     *  Queue of packets that will be send to the server.
     */
    std::queue<NetworkPackageContainer> packageQueue;

    const uint64_t &tick;  
    uint64_t lastReceivedTick = 0;

    /*
     *  Copies data from raw bytes into variable and moves data pointer to next variable
     */
    template <typename T>
    static T inline extractVariable(char **data, T &variable)
    {
      //Log::i("%p-%p of type %8s (size=%d bytes)\n", *data, *data+sizeof(T), typeid(T).name(), sizeof(T) );
      std::memcpy(&variable, *data, sizeof(T));
      *data += sizeof(T);
      return variable;
    }

    std::map<uint64_t, std::unique_ptr<Entity>> networkEntities;

    friend class Game;

  private:
    int sock{-1};
    struct sockaddr_in server;

    void inline deleteFirstPackage()
    {
      delete[] packageQueue.front().data;
      packageQueue.pop();
    }



};
