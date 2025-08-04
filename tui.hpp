#pragma once

#include <memory>
#include <ncurses.h>
#include <vector>

#include "player.hpp"

class TuiApp {
  int width;
  int height;
  std::vector<std::unique_ptr<Player>> &players;

public:
  TuiApp(std::vector<std::unique_ptr<Player>> &players);
  void setup();
  void update();
  void close();
  void setActiveWindow(const int &id);
};
