#pragma once

#include "Window.hpp"

extern int main();

namespace BladeEngine {
class World;

class Game {
public:
  Game();
  ~Game();

  static void Exit();

private:
  virtual void Update() = 0;

  void Run();
  void CleanUp();

  void UpdateInput();

private:
  bool m_ShouldExit = false;

  Window *m_Window;

  static Game *s_Instance;

  friend int ::main();
};

extern Game *CreateGameInstance();

} // namespace BladeEngine