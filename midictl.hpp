#pragma once

#include <rtmidi/RtMidi.h>
#include <array>

enum midiEvent
{
    NOTE_ON, NOTE_OFF, CC_EVENT
};

class Controller;

class MidiController{
    RtMidiIn midi;
//    void midiCallback(double deltatime, std::vector<unsigned char>* message, void* userData);
public:
    void setup(Controller* controller);
    void update();
};
