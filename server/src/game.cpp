#include "game.hpp"


Game::Game()
{

}

Game::~Game()
{

}

void Game::step()
{
  if (players.size() > 0 || tick < TICK_LIMIT)
  {
    // game logic
    for (auto const &p : players)
    {
      p.second->update(this);
    }
    
    tick++;
  }
}

Player *Game::getPlayer(uint64_t id)
{
  std::map<uint64_t,Player*>::iterator it;
  if ((it = players.find(id)) != players.end()) {
    return it->second;
  }
  return nullptr;
}

Player *Game::addPlayer(uint64_t id, PlayerData data, sockaddr inAddr)
{
  auto *p = new Player(data.position, data.rotation, data.velocity);
  p->setSockAddr(inAddr);
  printf("New player %zu.\n", id);
  p->lastTick = tick;
  this->players.insert(std::make_pair(id, p));
  return p;
}

void Game::removePlayer(uint64_t id)
{
  this->players.erase(id);
}


#define FRICTION 1.1F
#define ACCELERATION 0.1F
#define ROTATION_SPEED 0.05F

Player::Player(vec3<float> pos, vec3<float> rot, vec3<double> vel)
{
  this->position = pos;
  this->velocity = vel;
  this->rotation = rot;
}

void Player::update(Game *game)
{
  position.x += cos(-rotation.x)*velocity.x;
  position.z += sin(-rotation.x)*velocity.x;

  velocity.x /= FRICTION;
  if (fabs(velocity.x) < 0.001) velocity.x = 0;
}

bool Player::checkData(vec3<float> pos, vec3<float> rot, vec3<double> vel)
{
  return (pos == position && rot == rotation && vel == velocity);
}

void Player::setData(vec3<float> pos, vec3<float> rot, vec3<double> vel)
{
  this->position = pos;
  this->rotation = rot;
  this->velocity = vel;
}
