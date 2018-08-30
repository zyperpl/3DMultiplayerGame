#include <cstdlib>
#include <cstring>
#include <cassert>
#include <memory>
#include <algorithm>
 
#include "game.hpp"
#include "scene.hpp"
#include "log.hpp"
#include "view.hpp"
#include "config.hpp"
#include "input.hpp"
#include "texture.hpp"
#include "model.hpp"

#include "glm/glm.hpp"

Game::~Game()
{
  for (auto &e : staticEntities)  delete e;
  for (auto &e : dynamicEntities) delete e;
  for (auto &l : lights)          delete l;
}

bool Game::init()
{
  threads.push_back(std::thread(&Game::initNetworking, this));

  return initView();
}

bool Game::initView()
{
  // initialize view
  viewPtr.reset(new View());

  return true;
}

bool Game::initNetworking()
{
  // connect to server
  auto server = Config::getServerInfo();
  networkPtr.reset(new Network(server.ip, server.port, tick));
  networkPtr->send("Hello");

  return true;
}

bool Game::generateObjects()
{
  for (const auto &staticEntity : staticEntities)
  {
    staticEntity->generate();
  }

  for (const auto &dynamicEntity : dynamicEntities)
  {
    dynamicEntity->generate();
  }

  return true;
}

bool Game::load(Scene *scene)
{
  auto objects = scene->getSceneObjects();
  if (objects.size() <= 0) 
  {
    Log::w("Scene '%s' is empty.\n", scene->getName().c_str());
  }

  Log::i("Thread hardware concurrency: %d threads\n", std::thread::hardware_concurrency());

  for (size_t i = 0; i < objects.size(); i++)
  {
    this->loadSceneObject(*objects.at(i));  
  }

  return true;
}

void Game::run()
{
  // generate OpenGL vertex, normal, uv etc. buffers
  this->generateObjects();

  View *view = viewPtr.get();
  Input *input = inputPtr.get();

  glfwMakeContextCurrent(NULL);

  threads.push_back(std::thread(&Game::updateDynamicEntities, this) );
  threads.push_back(std::thread(&Game::drawEntities, this) );

  threads.push_back(std::thread(&Network::beginCommunication, networkPtr.get(), this) );

  GLFWwindow *window = view->window;

  glfwSetKeyCallback(window, &Input::keyCallback);
  glfwSetCursorPosCallback(window, &Input::cursorPositionCallback);
  glfwSetMouseButtonCallback(window, &Input::mouseButtonCallback);

  while (running && !glfwWindowShouldClose(window)) 
  {
    glfwPollEvents();

    if (input->key[Config::key(Exit)]) running = false;
  }
  if (!running) Log::i("Running interrupted.\n");
  if (!view->isOpen()) Log::i("Window closed!\n");

  running = false;
}

void Game::drawEntities()
{
  View *view = viewPtr.get();
  auto *camera = cameraPtr.get();
  auto *network = networkPtr.get();

  glfwMakeContextCurrent(view->window);

  while (running && view->isOpen())
  {
    while (!updateMutex.try_lock())
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(3));
    }

    // draw shadows into framebuffer 
    view->enableFramebuffer(1);
    view->clear();
    glCullFace(GL_BACK);
    static float near = -10.0F, far = 1000.0F;
    if (inputPtr->key[GLFW_KEY_B]) near-=0.1F;
    if (inputPtr->key[GLFW_KEY_N]) near+=0.1F;
    if (inputPtr->key[GLFW_KEY_Y]) far-=0.1F;
    if (inputPtr->key[GLFW_KEY_U]) far+=0.1F;

    static float shadowMapSize = 200.0F;
    glm::mat4 depthProjectionMatrix = 
      glm::ortho<float>(-shadowMapSize/2, shadowMapSize/2, 
                        -shadowMapSize/2, shadowMapSize/2, 
                        near, far);
    //glm::perspective<float>(glm::radians(60.0F), 1.0F, 0.1F, 10.0F);
    
    //XXX test: shadow animation
    static float a = 0.0;
    a+=0.01;
    const auto MAX_X_OFFSET = 30.0F;
    float xOffset = -(MAX_X_OFFSET/2)+cos(a)*MAX_X_OFFSET;

    glm::mat4 depthViewMatrix = glm::lookAt(
        { camera->position.x+xOffset, camera->position.y-20.0F,   camera->position.z+xOffset/2 }, 
        { camera->position.x+0.0F, camera->position.y+0.0F,    camera->position.z+0.0F }, 
        glm::vec3(0,1,0));
    glUseProgram(view->framebuffers[1]->programID);
    for (auto e : staticEntities)  
    { 
      if (e->scale.x > 10.0F) continue;
      e->draw(depthProjectionMatrix, depthViewMatrix, lights); 
    };
    //for (auto e : dynamicEntities) { e->draw(depthProjectionMatrix, depthViewMatrix, lights); };

    // draw models into framebuffer 
    view->enableFramebuffer(0);
    view->clear();
    glCullFace(GL_FRONT);
    glUseProgram(view->programID);
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(70.0F), 1.78F, 3.00F, 300.0F);
    //glm::mat4 projectionMatrix = depthProjectionMatrix;
    glm::vec3 cameraPosition(
        player->position.x-cos(-player->rotation.y)*CAMERA_DISTANCE, 
        player->position.y+CAMERA_DISTANCE, 
        player->position.z-sin(-player->rotation.y)*CAMERA_DISTANCE);

    glm::vec3 lookPosition(player->position.x, player->position.y, player->position.z);
    glm::mat4 viewMatrix = glm::lookAt(cameraPosition, lookPosition, glm::vec3(0,1,0) );

    GLint depthP = glGetUniformLocation(view->programID, "depthP");
    GLint depthV = glGetUniformLocation(view->programID, "depthV");
    glUniformMatrix4fv(depthP, 1, GL_FALSE, &depthProjectionMatrix[0][0]);
    glUniformMatrix4fv(depthV, 1, GL_FALSE, &depthViewMatrix[0][0]);
    GLint shadowMap = glGetUniformLocation(view->programID, "shadowMap");
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, view->framebuffers[1]->textureID);
    glUniform1i(shadowMap, 1);

    for (auto e : staticEntities)  { e->draw(projectionMatrix, viewMatrix, lights); };
    for (auto e : dynamicEntities) { e->draw(projectionMatrix, viewMatrix, lights); };
    for (const auto &p : network->networkEntities) { 
      if (p.second->isGenerated())
        p.second->draw(projectionMatrix, viewMatrix, lights); 
      else
        p.second->generate();
    };

    updateMutex.unlock();

    // draw framebuffer
    view->disableFramebuffer();
    view->clear();
    glCullFace(GL_FRONT);
    view->renderFramebuffer(0);

    // draw shadowmap
    glViewport(0, 0, 1920/5, 1080/4);
    view->renderFramebuffer(1);

    view->swapWindowBuffers();
  }
  glfwMakeContextCurrent(NULL);

} 

void Game::updateDynamicEntities()
{
  while (this->isRunning())
  {
    updateMutex.lock();

    // update dynamic objects
    for (size_t i = 0; i < dynamicEntities.size(); ++i)
    {
      dynamicEntities.at(i)->update(this);
    }
    this->tick++;

    updateMutex.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_INTERVAL_MS));
  }
}

bool Game::isRunning()
{
  return this->running;
}

void Game::joinThreads()
{
  Log::i("Waiting for %d threads 🤔...\n", threads.size());
  for (size_t i = 0; i < threads.size(); i++)
  {
    threads.at(i).join();
  }

  // removing threads from list
  for (int i = threads.size()-1; i >= 0; --i)
  {
    threads.erase(threads.begin()+i);
  }
}

void Game::loadSceneObject(const SceneObject &obj)
{ 
  Log::i("Loading entity \"%s\"..\n", obj.attributes.type.getName().c_str());
  if (obj.attributes.type.getName() == "static") {
    // loading static entities
    loadStaticEntity(obj);
  } else  
  if (obj.attributes.type == "player") {
    // loading player entity
    loadPlayer(obj);

    Log::i("Loaded player at position " VEC3F_STR ".\n", VEC3(obj.position));
  } else
  if (obj.attributes.type == "camera") {
    cameraPtr.reset(new SceneObject(obj) );

    Log::i("Loaded camera at " VEC3F_STR ".\n", VEC3(obj.position));
  } else
  if (obj.attributes.type == "light") {
    lights.push_back(std::move(new Light(obj)) );

    Log::i("Added light " VEC3F_STR " at position " VEC3F_STR ".\n",
        VEC3(obj.attributes.color), VEC3(obj.position));
  }
}

void Game::loadPlayer(const SceneObject &obj)
{
  player = new PlayerTank(obj);
  this->dynamicEntities.push_back(player);
}

void Game::loadStaticEntity(const SceneObject &obj)
{

  this->staticEntities.push_back
    (
      new StaticEntity
      (
        obj.model, 
        obj.texture, 
        obj.position, 
        obj.attributes.rotation,
        obj.attributes.scale, 
        obj.textureWrap
      )
    );

  Log::i("Added static entity (model=%s; texture=%s) at position " VEC3F_STR ".\n",
      obj.model.c_str(), obj.texture.c_str(), VEC3(obj.position));
}


