#include <cstdio>
#include <string>
#include <cstdlib>

#include "input.hpp"

int8_t Input::key[KEYS_NUM];
int8_t Input::mouseButton[MOUSE_BUTTONS_NUM];
vec2<double> Input::mousePosition;

Input::Input()
{
  memset(key, 0, KEYS_NUM);
  memset(mouseButton, 0, MOUSE_BUTTONS_NUM);

  mousePosition = { 0, 0 };
}


void Input::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{ 
  (void)(window); (void)(scancode); (void)(mode); // prevent warning

  Input::key[key] = action;
}


void Input::cursorPositionCallback(GLFWwindow *window, double xpos, double ypos)
{
  (void)(window);

  Input::mousePosition = { xpos, ypos };
}

void Input::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
  (void)(window); (void)(mods);

  Input::mouseButton[button] = action;
}
