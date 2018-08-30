#pragma once

class Input;

class Controller
{
  private:
    Input *input;
  public:
    Controller(Input *input);
};
