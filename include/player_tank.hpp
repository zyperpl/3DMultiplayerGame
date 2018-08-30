#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>

class View;
class Input;

#include "dynamic_entity.hpp"
#include "math.hpp"

#define PLAYERTANK_DEFAULT_MODEL   "models/vehicle.obj"
#define PLAYERTANK_DEFAULT_TEXTURE "models/propane_tank_red.png"
#define PLAYERTANK_DEFAULT_SCALE    2.6

struct SceneObject;

class PlayerTank : public DynamicEntity
{
  public:
    /*
     *  Creates player's tank with default parameters
     */
    PlayerTank();

    /*
     *  Creates player's tank based on specified Scene Object.
     */
    PlayerTank(const SceneObject &obj);

    /*
     *  Updates player's logic
     */
    void update(Game *game);

    inline vec3<double> getVelocity() { return this->velocity; }
    inline void setVelocity(vec3<double> v) { this->velocity = v; }
  private:
    vec3<double> velocity{{{0,0,0}}};
};
