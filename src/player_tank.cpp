#include <cmath>

#include "player_tank.hpp"
#include "log.hpp"
#include "scene.hpp"
#include "input.hpp"
#include "game.hpp"
#include "network.hpp"

#define FRICTION 1.1F
#define ACCELERATION 0.1F
#define ROTATION_SPEED 0.05F

PlayerTank::PlayerTank() 
  : DynamicEntity(PLAYERTANK_DEFAULT_MODEL, PLAYERTANK_DEFAULT_TEXTURE, {0, 0, 0}, {0, 0, 0}, {PLAYERTANK_DEFAULT_SCALE, PLAYERTANK_DEFAULT_SCALE, PLAYERTANK_DEFAULT_SCALE})
{
  
}

PlayerTank::PlayerTank(const SceneObject &obj) : DynamicEntity(obj)
{
  Log::i("Player created at %f;%f;%f\n", position.x, position.y, position.z);
}

void PlayerTank::update(Game *game) 
{
  auto input = game->getInput();

  // move forward/backward
  if (input->key[Config::key(Up)])    velocity.x += ACCELERATION;
  if (input->key[Config::key(Down)])  velocity.x -= ACCELERATION;
  
  // rotate
  if (input->key[Config::key(Left)])  rotation.y += ROTATION_SPEED;
  if (input->key[Config::key(Right)]) rotation.y -= ROTATION_SPEED;

  position.x += cos(-rotation.y)*velocity.x;
  position.z += sin(-rotation.y)*velocity.x;

  velocity.x /= FRICTION;
  if (fabs(velocity.x) < 0.001) velocity.x = 0;
 
  game->getNetwork()->addPacket(NetworkPackageType::PlayerInfo, { position, rotation, velocity });
}
