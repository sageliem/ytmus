#include "controller.hpp"
#include "player.hpp"

#include <chrono>
#include <functional>
#include <iostream>
#include <math.h>
#include <mutex>

bool operator>(const Event &ls, const Event &rs) { return ls.when > rs.when; }

// Create an instance of Controller holding pointer to array of Players
Controller::Controller(std::vector<std::unique_ptr<Player>> &players)
    : players{players}, scrubRate{0.01}, sched{} {
  start = std::chrono::high_resolution_clock::now();
}

// Called by main()
void Controller::update() {

  time = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - start);

  sched_mutex.lock();

  if (sched.empty()) {
    sched_mutex.unlock();
    return;
  }

  Event soonest = sched.top();
  //    std::cout << "Time: " << time.count() << "  soonest event: " <<
  //    soonest.when.count() << '\n';
  if (soonest.when <= time) {
    std::cout << "Running control event with scheduled time "
              << soonest.when.count() << "\n";
    // doControlEvent(soonest.eventType, soonest.controlValue);
    sched.pop();
    soonest.event();
  }

  sched_mutex.unlock();
}

// Handles a control event. Called by MidiController (rename to MidiHandler).
// TODO implement other Handler objects (OSC, keyboard control, etc)
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

// Perform event (called by update)
// void Controller::doControlEvent(controlEvent eventType, double
// controlValue)
// {
//   // Placeholder code to fix out of range issues TODO
//   // if (eventType == SELECT_BUFFER && controlValue == 0) return;
//
//   switch (eventType) {
//   case CHANGE_SPEED:
//     ctlSpeed(controlValue);
//     break;
//   case LOOP_START:
//     ctlLoopStart(controlValue);
//     break;
//   case LOOP_END:
//     ctlLoopEnd(controlValue);
//     break;
//   case LOOP_LENGTH:
//     ctlLoopLength(controlValue);
//     break;
//   case SEEK:
//     ctlSeek(controlValue);
//     break;
//   case SELECT_BUFFER:
//     ctlBufSelect(controlValue);
//     break;
//   case CHANGE_PITCH:
//     ctlPitch(controlValue);
//     break;
//   case CHANGE_VOLUME:
//     ctlVolume(controlValue);
//     break;
//   }
// }

// CONTROL METHODS
// These can be overloaded to push themselves to the scheduler queue by adding
// a millisecond delay Calls seek function on playerIndex player
void Controller::ctlSeek(int playerIndex, double controlValue) {
  std::cout << "ctlSeek: " << controlValue << '\n';
  //    players->at(playerIndex).seek( players->at(playerIndex).getDuration()
  //    * controlValue );
  players[playerIndex]->seek(players[playerIndex]->getDuration() *
                             controlValue);
}

// Calls loop start on player
void Controller::ctlLoopStart(int playerIndex, double controlValue) {
  players[playerIndex]->set_loop_start(players[playerIndex]->getDuration() *
                                       controlValue);
}

// Calls loop length on player
void Controller::ctlLoopLength(int playerIndex, double controlValue) {
  double maxLength = 16; // Intervals of 0.063 seconds per MIDI CC
  // controlValue
  players[playerIndex]->set_loop_length(controlValue * maxLength);
}

// Calls loop end
void Controller::ctlLoopEnd(int playerIndex, double controlValue) {
  players[playerIndex]->set_loop_end(players[playerIndex]->getDuration() *
                                     controlValue);
}

// Calls speed control
void Controller::ctlSpeed(int playerIndex, double controlValue) {
  double max_speed = 4.0;
  double speed =
      controlValue * (max_speed - 0.25) + 0.25; // Min speed is 0.25 with audio
  players[playerIndex]->set_rate(speed);
}

// Calls pitch control
void Controller::ctlPitch(int playerIndex, double controlValue) {
  // Map to integer range -12 - 12
  int semitones = static_cast<int>(controlValue * 24) - 12;
  players[playerIndex]->setPitch(semitones);
}

// Calls volume control
void Controller::ctlVolume(int playerIndex, double controlValue) {
  double volume = controlValue * 100;
  players[playerIndex]->setVolume(volume);
}

// Overloads
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

//
// SCRUB methods for relative controls (send -2<value<2)
//
void Controller::scrubSeek(int playerIndex, int controlValue) {
  std::unique_ptr<Player> &player = players[playerIndex];
  std::cout << "scrubSeek: " << controlValue << '\n';
  //    players->at(playerIndex).seek( players->at(playerIndex).getDuration()
  //    * controlValue );
  player->seek(player->getCurrentPos() +
               static_cast<double>(controlValue) * scrubRate);
}

// Calls loop start on player
void Controller::scrubLoopStart(int playerIndex, int controlValue) {
  std::unique_ptr<Player> &player = players[playerIndex];
  player->set_loop_start(player->getLoopStart() +
                         static_cast<double>(controlValue) * scrubRate);
}

// Calls loop length on player
void Controller::scrubLoopLength(int playerIndex, int controlValue) {
  std::unique_ptr<Player> &player = players[playerIndex];
  player->set_loop_length(player->getLoopLength() +
                          static_cast<double>(controlValue) * scrubRate);
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
}

// Calls pitch control
void Controller::scrubPitch(int playerIndex, int controlValue) {
  std::unique_ptr<Player> &player = players[playerIndex];
  if (controlValue > 0)
    player->setPitch(player->getPitch() + 1);
  else if (controlValue < 0)
    player->setPitch(player->getPitch() - 1);
}

// Calls volume control
void Controller::scrubVolume(int playerIndex, int controlValue) {
  std::unique_ptr<Player> &player = players[playerIndex];
  double volume = player->getVolume();
  if (controlValue > 0)
    volume++;
  else if (controlValue < 0)
    volume--;

  if ((volume > 0) && (volume < 100))
    player->setVolume(volume);
}
