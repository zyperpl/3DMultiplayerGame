#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>
#include <map>
#include <cmath>

#include <sys/socket.h>

#define UPDATE_INTERVAL_MS 30
#define TICK_LIMIT 30

#include <vector>

template <class T>
struct vec2
{
  T x;
  T y;
};

#define VEC2(v) v.x,v.y

template <class T>
struct vec3
{
  union {
    struct {
      T x;
      T y;
      T z;
    };
    struct {
      T r;
      T g;
      T b;
    };
  };

  bool operator==(const vec3 &v)
  {
    return v.x == x && v.y == y && v.z == z;
  }
};

#define VEC3(v) v.x,v.y,v.z

#define VEC3F_STR "%.2f;%.2f;%.2f"
#define VEC3F_STR "%.2f;%.2f;%.2f"

class Game;

struct __attribute__((packed)) PlayerData
{ 
  vec3<float> position;
  vec3<float> rotation;
  vec3<double> velocity;
};

class Player
{
  public:
    Player(vec3<float> pos, vec3<float> rot, vec3<double> vel);
    bool checkData(vec3<float> pos, vec3<float> rot, vec3<double> vel);
    void setData(vec3<float> pos, vec3<float> rot, vec3<double> vel);

    uint64_t inline getLastTick() { return this->lastTick; }
    void inline setLastTick(uint64_t t) { this->lastTick = t; }

    inline vec3<float> getPosition() { return this->position; }
    inline vec3<float> getRotation() { return this->rotation; }
    inline vec3<double> getVelocity() { return this->velocity; }

    void update(Game *game);
    inline void setSockAddr(sockaddr inAddr) { this->addr = inAddr; }
    inline sockaddr getSockAddr() { return this->addr; }
  private:
    vec3<float> position;
    vec3<float> rotation;
    vec3<double> velocity;
    
    /*
     *  Last game server tick when player was updated
     */
    uint64_t lastTick;

    sockaddr addr;

    friend class Game;
};

class Game
{
  public:
    Game();
    ~Game();

    /*
     * Simulates one tick of the game
     */
    void step();
  
    /*
     * Returns a player on specifier socket id
     */
    Player *getPlayer(uint64_t id);

    /*
     * Adds new player to the list
     */
    Player *addPlayer(uint64_t id, PlayerData data, sockaddr inAddr);

    /*
     * Removes player from the list
     */
    void removePlayer(uint64_t id);

    inline std::map<uint64_t, Player*> getPlayers() { return this->players; }
    inline uint64_t getTick() { return this->tick; }

  private:
    uint64_t tick{0}; 
    std::map<uint64_t, Player*> players;

};
