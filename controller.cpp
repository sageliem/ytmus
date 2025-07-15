#include "controller.hpp"
#include "player.hpp"
#include "tui.hpp"

#include <chrono>
#include <functional>
#include <iostream>
#include <math.h>
#include <mutex>

// comparison for sorting event queue
bool operator>(const Event &ls, const Event &rs) { return ls.when > rs.when; }

// Create an instance of Controller holding pointer to array of Players
Controller::Controller(std::vector<std::unique_ptr<Player>> &players,
                       std::unique_ptr<TuiApp> &ui)
    : players{players}, scrubRate{0.01}, ui{ui}, sched{} {
  start = std::chrono::high_resolution_clock::now();
}

// Called in main loop
void Controller::update() {

  time = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - start);

  sched_mutex.lock();

  // Update UI
  if (ui) {
    for (int i = 0; i < players.size(); i++) {
      ui->getWindow(i)->setTimePos(players[i]->getCurrentPos());
    }
  } else {
    for (int i = 0; i < players.size(); i++) {
      std::cout << "currentPos: " << players[i]->getCurrentPos();
    }
  }

  if (sched.empty()) {
    sched_mutex.unlock();
    return;
  }

  Event soonest = sched.top();
  if (soonest.when <= time) {
    std::cout << "Running control event with scheduled time "
              << soonest.when.count() << "\n";
    sched.pop();
    soonest.event();
  }

  sched_mutex.unlock();
}

// Handles a control event, called by handler objects
void Controller::pushEvent(std::function<void()> event, int delayMillis) {
  time = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - start);

  sched_mutex.lock();
  sched.push(Event{
      .event = event,
      .when = time + std::chrono::milliseconds(delayMillis),
  });
  sched_mutex.unlock();
}

// CONTROL METHODS
// These can be overloaded to push themselves to the scheduler queue by adding
// a millisecond delay.
void Controller::ctlSeek(int playerIndex, double controlValue) {
  double seekPos = players[playerIndex]->getDuration() * controlValue;
  players[playerIndex]->seek(seekPos);
  if (ui)
    ui->getWindow(playerIndex)->setTimePos(seekPos);
}

void Controller::ctlLoopStart(int playerIndex, double controlValue) {
  double loopStart = players[playerIndex]->getDuration() * controlValue;
  players[playerIndex]->set_loop_start(loopStart);
  if (ui)
    ui->getWindow(playerIndex)->setLoopStart(loopStart);
}

void Controller::ctlLoopLength(int playerIndex, double controlValue) {
  double loopLength = 16 * controlValue;
  players[playerIndex]->set_loop_length(loopLength);
  if (ui)
    ui->getWindow(playerIndex)->setLoopLength(loopLength);
}

void Controller::ctlLoopEnd(int playerIndex, double controlValue) {
  players[playerIndex]->set_loop_end(players[playerIndex]->getDuration() *
                                     controlValue);
}

void Controller::ctlSpeed(int playerIndex, double controlValue) {
  double max_speed = 4.0;
  double speed =
      controlValue * (max_speed - 0.25) + 0.25; // Min speed is 0.25 with audio
  players[playerIndex]->set_rate(speed);
  if (ui)
    ui->getWindow(playerIndex)->setSpeed(speed);
}

void Controller::ctlPitch(int playerIndex, double controlValue) {
  // Map to integer range -12 - 12
  int semitones = static_cast<int>(controlValue * 24) - 12;
  players[playerIndex]->setPitch(semitones);
  double doublesemitones = static_cast<double>(semitones); // TODO fix this
  if (ui)
    ui->getWindow(playerIndex)->setPitch(doublesemitones);
}

void Controller::ctlVolume(int playerIndex, double controlValue) {
  double volume = controlValue * 100;
  players[playerIndex]->setVolume(volume);
  if (ui)
    ui->getWindow(playerIndex)->setVolume(volume);
}

//
// SCRUB methods for relative controls (send -2<value<2)
//
void Controller::scrubSeek(int playerIndex, int controlValue) {
  std::unique_ptr<Player> &player = players[playerIndex];
  std::cout << "scrubSeek: " << controlValue << '\n';
  //    players->at(playerIndex).seek( players->at(playerIndex).getDuration()
  //    * controlValue );
  double pos =
      player->getCurrentPos() + static_cast<double>(controlValue) * scrubRate;
  player->seek(pos);
  if (ui)
    ui->getWindow(playerIndex)->setTimePos(pos);
}

// Calls loop start on player
void Controller::scrubLoopStart(int playerIndex, int controlValue) {
  std::unique_ptr<Player> &player = players[playerIndex];
  double loopStart =
      player->getLoopStart() + static_cast<double>(controlValue) * scrubRate;
  player->set_loop_start(loopStart);
  if (ui)
    ui->getWindow(playerIndex)->setLoopStart(loopStart);
}

// Calls loop length on player
void Controller::scrubLoopLength(int playerIndex, int controlValue) {
  std::unique_ptr<Player> &player = players[playerIndex];
  double length =
      player->getLoopLength() + static_cast<double>(controlValue) * scrubRate;
  player->set_loop_length(length);
  if (ui)
    ui->getWindow(playerIndex)->setLoopLength(length);
}

// Calls loop end
void Controller::scrubLoopEnd(int playerIndex, int controlValue) {
  std::unique_ptr<Player> &player = players[playerIndex];
  player->set_loop_end(player->getLoopEnd() +
                       static_cast<double>(controlValue) * scrubRate);
}

// Calls speed control
void Controller::scrubSpeed(int playerIndex, int controlValue) {
  double max_speed = 4.0;
  double min_speed = 0.25;
  double speed = players[playerIndex]->getSpeed();
  speed += static_cast<double>(controlValue) * scrubRate;
  if (speed > max_speed)
    speed = max_speed;
  else if (speed < min_speed)
    speed = min_speed;
  players[playerIndex]->set_rate(speed);
  if (ui)
    ui->getWindow(playerIndex)->setSpeed(speed);
}

// Calls pitch control
void Controller::scrubPitch(int playerIndex, int controlValue) {
  std::unique_ptr<Player> &player = players[playerIndex];
  int pitch = player->getPitch();
  if (controlValue > 0)
    pitch++;
  else if (controlValue < 0)
    pitch--;
  player->setPitch(pitch);
  if (ui) {
    double pitchdouble = static_cast<double>(pitch);
    ui->getWindow(playerIndex)->setPitch(pitchdouble);
  }
}

// Calls volume control
void Controller::scrubVolume(int playerIndex, int controlValue) {
  std::unique_ptr<Player> &player = players[playerIndex];
  double volume = player->getVolume();
  if (controlValue > 0)
    volume++;
  else if (controlValue < 0)
    volume--;

  if ((volume > 0) && (volume < 100)) {
    player->setVolume(volume);
    ui->getWindow(playerIndex)->setVolume(volume);
  }
}

//
// Overloads for scheduled events
//
void Controller::ctlSeek(int playerIndex, double controlValue, int delay) {
  pushEvent([=]() { ctlSeek(playerIndex, controlValue); }, delay);
}

void Controller::ctlLoopLength(int playerIndex, double controlValue,
                               int delay) {
  pushEvent([=]() { ctlLoopLength(playerIndex, controlValue); }, delay);
}

void Controller::ctlLoopStart(int playerIndex, double controlValue, int delay) {
  pushEvent([=]() { ctlLoopStart(playerIndex, controlValue); }, delay);
}

void Controller::ctlLoopEnd(int playerIndex, double controlValue, int delay) {
  pushEvent([=]() { ctlLoopEnd(playerIndex, controlValue); }, delay);
}

void Controller::ctlSpeed(int playerIndex, double controlValue, int delay) {
  pushEvent([=]() { ctlSpeed(playerIndex, controlValue); }, delay);
}

void Controller::ctlPitch(int playerIndex, double controlValue, int delay) {
  pushEvent([=]() { ctlPitch(playerIndex, controlValue); }, delay);
}

void Controller::ctlVolume(int playerIndex, double controlValue, int delay) {
  pushEvent([=]() { ctlVolume(playerIndex, controlValue); }, delay);
}
