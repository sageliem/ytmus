#pragma once

#include <rtmidi/RtMidi.h>

enum midiEvent { NOTE_ON, NOTE_OFF, CC_EVENT };

void midiCallback(double deltatime, std::vector<unsigned char> *message,
                  void *userData);

class Controller;

class MidiHandler {
  RtMidiIn midi;
  Controller *controller;
  bool relativeKnobs;
  int activePlayer;
  void midiCCEventRelative(int midiCCNumber, int midiCCValue);
  //    void midiCallback(double deltatime, std::vector<unsigned char>* message,
  //    void* userData);
public:
  void setup(Controller *controller);
  void update();

  void setRelativeKnobs(bool knobSetting);
  void setActivePlayer(int playerIndex);
  void midiCCEvent(int midiCCNumber, int midiCCValue);
};
