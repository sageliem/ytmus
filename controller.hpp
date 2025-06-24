// Controller.hpp
//
#pragma once

#include "types.hpp"
#include <array>
#include <chrono>
#include <functional>
#include <mutex>
#include <queue>

class Player; // Forward declaration of player

// Event class for schedule queue
typedef struct Event {
  std::function<void()> event;
  std::chrono::milliseconds when;
} Event;

class Controller {
  std::vector<std::unique_ptr<Player>> &players;
  std::chrono::time_point<std::chrono::high_resolution_clock> start;
  std::chrono::milliseconds time;
  std::mutex sched_mutex;
  std::unique_lock<std::mutex> sched_lock;
  double scrubRate;

  std::priority_queue<Event, std::vector<Event>, std::greater<Event>> sched;

public:
  Controller(std::vector<std::unique_ptr<Player>> &players);

  void ctlSeek(int playerIndex, double value);
  void ctlLoopStart(int playerIndex, double value);
  void ctlLoopEnd(int playerIndex, double value);
  void ctlLoopLength(int playerIndex, double value);
  void ctlSpeed(int playerIndex, double value);
  void ctlBufSelect(int playerIndex, int value);
  void ctlPitch(int playerIndex, double value);
  void ctlVolume(int playerIndex, double value);

  // Overloaded control methods push themselves to queue
  void ctlSeek(int playerIndex, double value, int delay);
  void ctlLoopStart(int playerIndex, double value, int delay);
  void ctlLoopEnd(int playerIndex, double value, int delay);
  void ctlLoopLength(int playerIndex, double value, int delay);
  void ctlSpeed(int playerIndex, double value, int delay);
  void ctlBufSelect(int playerIndex, int value, int delay);
  void ctlPitch(int playerIndex, double value, int delay);
  void ctlVolume(int playerIndex, double value, int delay);

  void scrubSeek(int playerIndex, int value);
  void scrubLoopStart(int playerIndex, int value);
  void scrubLoopEnd(int playerIndex, int value);
  void scrubLoopLength(int playerIndex, int value);
  void scrubSpeed(int playerIndex, int value);
  void scrubBufSelect(int playerIndex, int value);
  void scrubPitch(int playerIndex, int value);
  void scrubVolume(int playerIndex, int value);

  void pushEvent(std::function<void()> event, int delayMillis);

  void update();
};
