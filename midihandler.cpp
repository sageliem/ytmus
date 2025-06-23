#include <iostream>

#include "midihandler.hpp"

#include "controller.hpp"

// Called each MIDI event
// TODO refactor into MidiHandler class
void midiCallback(double deltatime, std::vector<unsigned char> *message,
                  void *userData) {
  // userData is a reference to the handler that calls this
  MidiHandler *parentHandler = static_cast<MidiHandler *>(userData);

  // Filter out nonstandard message types
  if (message->size() != 3)
    return;

  // TODO clean up types
  unsigned int status =
      static_cast<unsigned int>((*message)[0]); // Message type
  unsigned int status_shift = status >> 4;
  int midiNumber = (*message)[1]; // Note/CC number
  int midiValue = (*message)[2];  // Note/CC value

  switch (status_shift) {

  // CC message
  case 0xB:
    //           std::cout << "CC Input\n";
    parentHandler->midiCCEvent(midiNumber, midiValue);
    break;

  // NOTE_ON
  case 0x9:
    if (midiNumber >= 48 && midiNumber <= 56)
      parentHandler->setActivePlayer(static_cast<int>(midiNumber) - 48);
    //            std::cout << "NoteOn number " << number << "\n";
    break;

  // NOTE_OFF
  case 0x8:
    //      std::cout << "Note Off\n";
    break;
  }
}

// Initialize midi controller, select port
void MidiHandler::setup(Controller *controller) {
  // Get port count
  unsigned int nPorts{midi.getPortCount()};
  std::cout << "There are " << nPorts << " MIDI input sources available.\n";

  std::string portName; // Stores current port name

  this->controller = controller;

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

  // Get port selection from user
  unsigned int portNum;
  std::cout << "Select a port: ";
  std::cin >> portNum;

  // Attempt to open selected port
  try {
    midi.openPort(portNum);
  } catch (RtMidiError &err) {
    err.printMessage();
    return;
  }

  std::cout << "Listening on port " << midi.getPortName() << '\n';

  // Ignore special messages (reduces number of callbacks)
  midi.ignoreTypes(true, true, true);

  // Set callback function, pass pointer to self
  midi.setCallback(&midiCallback, this);
}

// Placeholder, not needed currently
void MidiHandler::update() { return; }

// Helper for midiCallback
// Hardcoded to Arturia Minilab Mkii
void MidiHandler::midiCCEvent(int midiCCNumber, int midiCCValue) {
  double normCCValue = static_cast<double>(midiCCValue) / 127.0;
  switch (midiCCNumber) {
  // Knob 8 SEEK
  case 75:
    controller->ctlSeek(activePlayer, normCCValue);
    break;
  // Knob 7 SPEED
  case 73:
    controller->ctlSpeed(activePlayer, normCCValue);
    break;
  // Knob 2 LOOP_START
  case 74:
    controller->ctlLoopStart(activePlayer, normCCValue);
    break;
  // Knob 3 LOOP_LENGTH
  case 71:
    controller->ctlLoopLength(activePlayer, normCCValue);
    break;
  // Knob 6 PITCH
  case 93:
    controller->ctlPitch(activePlayer, normCCValue);
    break;
  // Knob 10 VOLUME
  case 18:
    controller->ctlVolume(activePlayer, normCCValue);
    break;
  }
}
