#pragma once

#include <rtmidi/RtMidi.h>
#include <unordered_map>

enum midiEvent { NOTE_ON, NOTE_OFF, CC_EVENT };

void midiCallback(double deltatime, std::vector<unsigned char> *message,
                  void *userData);

class Controller;

class MidiHandler {
  RtMidiIn midi;
  Controller *controller;
  bool relativeKnobs;
  int activePlayer;

  enum Mapping {
    SEEK_MAP = 75,
    SPEED_MAP = 73,
    LOOPSTART_MAP = 74,
    LOOPLENGTH_MAP = 71,
    PITCH_MAP = 93,
    VOLUME_MAP = 18,
    PITCH0_MAP = 60,
    BUFFER0_MAP = 0
  };
  std::unordered_map<Mapping, int> mapping;

  void midiCCEventRelative(int midiCCNumber, int midiCCValue);
  //    void midiCallback(double deltatime, std::vector<unsigned char>* message,
  //    void* userData);
public:
  MidiHandler(Controller *controller);
  void setup();
  void setRelativeKnobs(bool knobSetting);
  void setActivePlayer(int playerIndex);
  void midiCCEvent(int midiCCNumber, int midiCCValue);
  void showConfig();
};
