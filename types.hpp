// Enumerations used throughout project

#pragma once

#include <rtmidi/RtMidi.h>
enum controlEvent
{
    SELECT_BUFFER, SEEK, LOOP_START,
    LOOP_END, CHANGE_SPEED, CHANGE_PITCH, CHANGE_VOLUME,
    LOOP_LENGTH
};

enum controllerType
{
    OSC, MIDI
};
