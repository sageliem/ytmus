#include "controller.hpp"
#include "player.hpp"

#include <chrono>
#include <functional>
#include <iostream>
#include <math.h>
#include <mutex>

bool operator>(const Event &ls, const Event &rs) { return ls.when > rs.when; }

// Create an instance of Controller holding pointer to array of Players
Controller::Controller(std::array<Player, 8> *players)
    : players{players}, sched{} {
  start = std::chrono::high_resolution_clock::now();
}

// Called by main()
void Controller::update() {
  sched_mutex.lock();

  time = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - start);

  if (sched.empty())
    return;

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

void Controller::lockQueue() { sched_mutex.lock(); }

void Controller::unlockQueue() { sched_mutex.unlock(); }

// Handles a control event. Called by MidiController (rename to MidiHandler).
// TODO implement other Handler objects (OSC, keyboard control, etc)
void Controller::pushEvent(std::function<void()> event, int delayMillis) {
  time = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - start);

  lockQueue();
  sched.push(Event{
      .event = event,
      .when = time + std::chrono::milliseconds(delayMillis),
  });
  unlockQueue();
}

// Perform event (called by update)
// void Controller::doControlEvent(controlEvent eventType, double controlValue)
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
// These can be overloaded to push themselves to the scheduler queue by adding a
// millisecond delay Calls seek function on playerIndex player
void Controller::ctlSeek(int playerIndex, double controlValue) {
  std::cout << "ctlSeek: " << controlValue << '\n';
  //    players->at(playerIndex).seek( players->at(playerIndex).getDuration() *
  //    controlValue );
  players->at(playerIndex).seek(controlValue);
}

// Calls loop start on player
void Controller::ctlLoopStart(int playerIndex, double controlValue) {
  players->at(playerIndex)
      .set_loop_start(players->at(playerIndex).getDuration() * controlValue);
}

void Controller::ctlLoopStart(int playerIndex, double controlValue, int delay) {
  pushEvent([=]() { ctlLoopStart(playerIndex, controlValue); }, delay);
}

// Calls loop length on player
void Controller::ctlLoopLength(int playerIndex, double controlValue) {
  double maxLength = 16; // Intervals of 0.063 seconds per MIDI CC controlValue
  players->at(playerIndex).set_loop_length(controlValue * maxLength);
}

// Calls loop end
void Controller::ctlLoopEnd(int playerIndex, double controlValue) {
  players->at(playerIndex)
      .set_loop_end(players->at(playerIndex).getDuration() * controlValue);
}

// Calls speed control
void Controller::ctlSpeed(int playerIndex, double controlValue) {
  double max_speed = 4.0;
  double speed =
      controlValue * (max_speed - 0.25) + 0.25; // Min speed is 0.25 with audio
  players->at(playerIndex).set_rate(speed);
}

// Calls pitch control
void Controller::ctlPitch(int playerIndex, double controlValue) {
  // Map to integer range -12 - 12
  int semitones = static_cast<int>(controlValue * 24) - 12;
  players->at(playerIndex).setPitch(semitones);
}

// Calls volume control
void Controller::ctlVolume(int playerIndex, double controlValue) {
  double volume = controlValue * 100;
  players->at(playerIndex).setVolume(volume);
}

// Overloads
void Controller::ctlSeek(int playerIndex, double controlValue, int delay) {
  pushEvent([=]() { ctlSeek(playerIndex, controlValue); }, delay);
}

void Controller::ctlLoopLength(int playerIndex, double controlValue,
                               int delay) {
  pushEvent([=]() { ctlLoopLength(playerIndex, controlValue); }, delay);
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

// Changes currently selected buffer
// void Controller::ctlBufSelect(int controlValue) {
//   // Handle Arturia minilab presets (Placeholder)
//   // TODO abstraction
//   std::cout << "BufferSelect " << controlValue << '\n';
//   active = static_cast<int>(std::floor(controlValue));
//   //    std::cout << "Set active buffer to " << controlValue -48 << '\n';
// }
