#include <iostream>

#include "include/inih/cpp/INIReader.h"

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
    if (midiNumber >= 48 && midiNumber <= 56)
      parentHandler->setActivePlayer(static_cast<int>(midiNumber) - 48);
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
  INIReader ini("config.ini");

  if (ini.ParseError() < 0) {
    std::cout << "Could not read configuration config.ini\n";
    return;
  }

  setRelativeKnobs(ini.GetBoolean("midi", "relative", false));

  std::cout << "Config loaded from config.ini\n";
  std::cout << "Seek Knob: " << ini.GetInteger("midi", "seek", 0) << '\n';

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
  switch (midiCCNumber) {
  case SEEK_MAP:
    controller->ctlSeek(activePlayer, normCCValue);
    break;
  case SPEED_MAP:
    controller->ctlSpeed(activePlayer, normCCValue);
    break;
  case LOOPSTART_MAP:
    controller->ctlLoopStart(activePlayer, normCCValue);
    break;
  case LOOPLENGTH_MAP:
    controller->ctlLoopLength(activePlayer, normCCValue);
    break;
  case PITCH_MAP:
    controller->ctlPitch(activePlayer, normCCValue);
    break;
  case VOLUME_MAP:
    controller->ctlVolume(activePlayer, normCCValue);
    break;
  }
}

void MidiHandler::midiCCEventRelative(int midiCCNumber, int midiCCValue) {
  switch (midiCCNumber) {
  case SEEK_MAP:
    controller->scrubSeek(activePlayer, midiCCValue - 64);
    break;
  case SPEED_MAP:
    controller->scrubSpeed(activePlayer, midiCCValue - 64);
    break;
  case LOOPSTART_MAP:
    controller->scrubLoopStart(activePlayer, midiCCValue - 64);
    break;
  case LOOPLENGTH_MAP:
    controller->scrubLoopLength(activePlayer, midiCCValue - 64);
    break;
  case PITCH_MAP:
    controller->scrubPitch(activePlayer, midiCCValue - 64);
    break;
  case VOLUME_MAP:
    controller->scrubVolume(activePlayer, midiCCValue - 64);
    break;
  }
}
