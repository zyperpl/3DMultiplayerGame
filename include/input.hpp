#pragma once

#include "view.hpp"
#include "math.hpp"
#include "log.hpp"
#include "config.hpp"

#define KEYS_NUM 512
#define MOUSE_BUTTONS_NUM 5

class Input
{
  public:
    Input();
    static int8_t key[KEYS_NUM];
    static int8_t mouseButton[MOUSE_BUTTONS_NUM];
    static vec2<double> mousePosition;

    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
    static void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos);
    static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);


};
