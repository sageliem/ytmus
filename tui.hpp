#pragma once

#include "player.hpp"
#include <array>
#include <memory>
#include <ncurses.h>
#include <string>

class BufferWindow {
  WINDOW *win;
  std::unique_ptr<Player> &p;
  const char id;
  std::string name;
  int height;
  int width;
  int x;
  int y;

public:
  BufferWindow(const char id);
  void setup(std::unique_ptr<Player> &p_init, const int h_init,
             const int w_init, const int y_init, const int x_init);
  void load_player(std::unique_ptr<Player> &p_new);
  void update();
  void close();
};

class TuiApp {
  int width;
  int height;
  std::array<std::unique_ptr<BufferWindow>, 8> windows;

public:
  TuiApp();
  void setup();
  void update();
  void close();
  void load_buffer(const int id, std::unique_ptr<Player> &p);
  void setActiveBuffer(const int id);
};
