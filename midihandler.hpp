#pragma once

#include <rtmidi/RtMidi.h>

enum midiEvent { NOTE_ON, NOTE_OFF, CC_EVENT };

class Controller;

class MidiHandler {
  RtMidiIn midi;
  Controller *controller;
  int activePlayer;
  //    void midiCallback(double deltatime, std::vector<unsigned char>* message,
  //    void* userData);
public:
  void setup(Controller *controller);
  void update();

  void setActivePlayer(int playerIndex);
  void midiCCEvent(int midiCCNumber, int midiCCValue);
};
