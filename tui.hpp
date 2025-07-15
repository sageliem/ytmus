#pragma once

#include <memory>
#include <ncurses.h>
#include <string>
#include <vector>

class PlayerWindow {
  WINDOW *win;
  const int id;
  std::string name;
  bool active;
  int height;
  int width;
  int x;
  int y;
  double timePos;
  double loopStart;
  // double loopLength;
  double loopEnd;
  double speed;
  double pitch;
  double volume;

public:
  PlayerWindow(const int id);
  void setPos(const int h_init, const int w_init, const int y_init,
              const int x_init);

  void setName(const std::string &name);
  void setTimePos(const double &pos);
  void setLoopStart(const double &start);
  void setLoopLength(const double &loopLength);
  void setSpeed(const double &pos);
  void setPitch(const double &pitch);
  void setVolume(const double &volume);
  void setActive(bool opt);

  void update();
  void close();
};

class TuiApp {
  int width;
  int height;
  std::vector<std::unique_ptr<PlayerWindow>> windows;

public:
  TuiApp();
  void setup();
  void update();
  void close();
  void setActiveWindow(const int &id);
  std::unique_ptr<PlayerWindow> &getWindow(const int &id);
};
