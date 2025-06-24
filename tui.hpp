#pragma once

#include <memory>
#include <ncurses.h>
#include <string>
#include <vector>

// class BufferWindow {
//   WINDOW *win;
//   std::unique_ptr<Player> &p;
//   const char id;
//   std::string name;
//   int height;
//   int width;
//   int x;
//   int y;
//
// public:
//   BufferWindow(const char id);
//   void setup(std::unique_ptr<Player> &p_init, const int h_init,
//              const int w_init, const int y_init, const int x_init);
//   void load_player(std::unique_ptr<Player> &p_new);
//   void update();
//   void close();
// };
//

class PlayerWindow {
  WINDOW *win;
  const int id;
  std::string name;
  int height;
  int width;
  int x;
  int y;
  double timePos;
  double loopStart;
  double loopEnd;
  double speed;
  double pitch;
  double volume;

public:
  PlayerWindow(const char id);
  void setPos(const int h_init, const int w_init, const int y_init,
              const int x_init);

  void setName(std::string name);
  void setTimePos(double pos);
  void setLoop(double start, double end);
  void setSpeed(double pos);
  void setPitch(double pitch);
  void setVolume(float pos);

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
  const std::unique_ptr<PlayerWindow> &getWindow(const int &id);
};
