#include <iostream>

#include "include/inicpp/inicpp.h"

#include "midihandler.hpp"

#include "controller.hpp"

// Called each MIDI event
// TODO refactor into MidiHandler class
void midiCallback(double deltatime, std::vector<unsigned char> *message,
                  void *userData) {
  MidiHandler *parentHandler = static_cast<MidiHandler *>(userData);

  if (message->size() != 3)
    return;

  unsigned int status = static_cast<unsigned int>((*message)[0]);
  unsigned int status_upper = status >> 4; // Upper 4 digits of status byte
  unsigned int midiNumber = (*message)[1]; // Note/CC number
  unsigned int midiValue = (*message)[2];  // Note/CC value

  switch (status_upper) {
  case 0xB: // CC message
    parentHandler->midiCCEvent(midiNumber, midiValue);
    break;
  case 0x9: // NOTE_ON
    if (midiNumber >= 36 && midiNumber <= 48)
      parentHandler->setActivePlayer(static_cast<int>(midiNumber) - 36);
    // if (midiNumber >= 0 && midiNumber <= 7)
    //   parentHandler->setActivePlayer(static_cast<int>(midiNumber));
    break;
  case 0x8: // NOTE_OFF
    break;
  }
}

MidiHandler::MidiHandler(Controller *controller)
    : midi{}, controller{controller}, activePlayer{0} {}

// Initialize midi controller, select port
void MidiHandler::setup() {
  // Read configuration
  ini::IniFile cfg;
  cfg.load("config.ini");

  setRelativeKnobs(cfg["midi"]["relative"].as<bool>());

  mapping[SEEK_MAP] = cfg["midi"]["seek"].as<int>();
  mapping[SPEED_MAP] = cfg["midi"]["speed"].as<int>();
  mapping[LOOPSTART_MAP] = cfg["midi"]["loopstart"].as<int>();
  mapping[LOOPLENGTH_MAP] = cfg["midi"]["looplength"].as<int>();
  mapping[PITCH_MAP] = cfg["midi"]["pitch"].as<int>();
  mapping[VOLUME_MAP] = cfg["midi"]["volume"].as<int>();
  mapping[PITCH0_MAP] = cfg["midi"]["pitch0"].as<int>();
  mapping[BUFFER0_MAP] = cfg["midi"]["buffer0"].as<int>();

  std::cout << "Config loaded from config.ini\n";
  showConfig();

  // Get port count
  unsigned int nPorts{midi.getPortCount()};
  std::cout << "There are " << nPorts << " MIDI input sources available.\n";

  std::string portName; // Stores current port name

  // List available ports
  for (unsigned int i = 0; i < nPorts; i++) {
    try {
      portName = midi.getPortName(i);
    } catch (RtMidiError &error) {
      error.printMessage();
      return;
    }

    std::cout << "  Input Port #" << i << ": " << portName << '\n';
  }

  // Open a port
  unsigned int portNum;
  std::cout << "Select a port: ";
  std::cin >> portNum;

  try {
    midi.openPort(portNum);
  } catch (RtMidiError &err) {
    err.printMessage();
    return;
  }

  std::cout << "Listening on port " << midi.getPortName() << '\n';

  // set up RtMidi callback
  midi.ignoreTypes(true, true, true); // Ignore special messages
  midi.setCallback(&midiCallback, this);
}

void MidiHandler::setRelativeKnobs(bool knobSetting) {
  relativeKnobs = knobSetting;
}

void MidiHandler::setActivePlayer(int playerIndex) {
  activePlayer = playerIndex;
}

// called by midiCallback
void MidiHandler::midiCCEvent(int midiCCNumber, int midiCCValue) {

  if (relativeKnobs) {
    midiCCEventRelative(midiCCNumber, midiCCValue);
  }

  double normCCValue = static_cast<double>(midiCCValue) / 127.0;
  if (midiCCNumber == mapping[SEEK_MAP])
    controller->ctlSeek(activePlayer, normCCValue);
  else if (midiCCNumber == mapping[SPEED_MAP])
    controller->ctlSpeed(activePlayer, normCCValue);
  else if (midiCCNumber == mapping[LOOPSTART_MAP])
    controller->ctlLoopStart(activePlayer, normCCValue);
  else if (midiCCNumber == mapping[LOOPLENGTH_MAP])
    controller->ctlLoopLength(activePlayer, normCCValue);
  else if (midiCCNumber == mapping[PITCH_MAP])
    controller->ctlPitch(activePlayer, normCCValue);
  else if (midiCCNumber == mapping[VOLUME_MAP])
    controller->ctlVolume(activePlayer, normCCValue);
}

void MidiHandler::midiCCEventRelative(int midiCCNumber, int midiCCValue) {
  if (midiCCNumber == mapping[SEEK_MAP])
    controller->scrubSeek(activePlayer, midiCCValue - 64);
  else if (midiCCNumber == mapping[SPEED_MAP])
    controller->scrubSpeed(activePlayer, midiCCValue - 64);
  else if (midiCCNumber == mapping[LOOPSTART_MAP])
    controller->scrubLoopStart(activePlayer, midiCCValue - 64);
  else if (midiCCNumber == mapping[LOOPLENGTH_MAP])
    controller->scrubLoopLength(activePlayer, midiCCValue - 64);
  else if (midiCCNumber == mapping[PITCH_MAP])
    controller->scrubPitch(activePlayer, midiCCValue - 64);
  else if (midiCCNumber == mapping[VOLUME_MAP])
    controller->scrubVolume(activePlayer, midiCCValue - 64);
}

void MidiHandler::showConfig() {
  for (auto midimap : mapping) {
    std::cout << midimap.first << ": " << midimap.second << std::endl;
  }
}
