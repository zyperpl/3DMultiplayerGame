#pragma once

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <array>

#include "math.hpp"
#include "view.hpp"

class Entity;
class Scene;
class Input;

struct SceneObject;
struct Light;

#include "static_entity.hpp"
#include "dynamic_entity.hpp"
#include "input.hpp"
#include "network.hpp"

#include "player_tank.hpp"

#define CAMERA_DISTANCE 20.0F

class Game
{
  public:
    Game() = default;
    ~Game();

    /*
     *  Load SceneObjects resources into memory
     */
    bool load(Scene *scene);

    /*
     *  Initializes game components
     */
    bool init();

    /*
     *  Waits for threads to finish
     */
    void joinThreads();

    /*
     *  Starts game threads
     */
    void run();

    /*
     *  Returns true if game is running
     */
    bool isRunning();

    /*
     *  Returns number of update tick
     */
    uint64_t inline getTick() { return this->tick; }

    /*
     *  Returns keyboard/mouse input class
     */
    Input inline *getInput() { return this->inputPtr.get(); }

    /*
     *  Returns network class
     */
    Network inline *getNetwork() { return this->networkPtr.get(); }

  private:
    PlayerTank *player{nullptr};
    std::unique_ptr<SceneObject> cameraPtr;

    std::unique_ptr<Network> networkPtr;
    std::unique_ptr<View> viewPtr;
    std::unique_ptr<Input> inputPtr{new Input()};

    std::vector<std::thread> threads;
    std::mutex updateMutex;

    std::vector<StaticEntity*> staticEntities;
    std::vector<DynamicEntity*> dynamicEntities;
    std::vector<Light*> lights;

    bool running{true};
    uint64_t tick{0};

    void loadStaticEntity(const SceneObject &obj);
    void loadPlayer(const SceneObject &obj);

    void loadSceneObject(const SceneObject &obj);
    bool initView();
    bool initNetworking();
    bool generateObjects();

    void updateDynamicEntities();
    void drawEntities();

    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
};
