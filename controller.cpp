#include "controller.hpp"
#include "player.hpp"

#include <iostream>

// Create an instance of Controller holding pointer to array of Players
Controller::Controller(std::array<Player, 8>* players)
  : players { players },
    active { 0 }
{}

// Handles a control event. Called by MidiController (rename to MidiHandler). TODO implement other Handler objects (OSC, keyboard control, etc)
void Controller::handleControlEvent(controlEvent type, int value)
{
    // Placeholder code to fix out of range issues
    if (type == SELECT_BUFFER && value == 0) return; 

    double normalizedValue = normalizeMidiValue( value );

    switch ( type )
    {
    case CHANGE_SPEED:
        ctlSpeed( normalizedValue );
        break;
    case LOOP_START:
        ctlLoopStart( normalizedValue );
        break;
    case LOOP_END:
        ctlLoopEnd( normalizedValue );
        break;
    case LOOP_LENGTH:
        ctlLoopLength( normalizedValue );
        break;
    case SEEK:
        ctlSeek( normalizedValue );
        break;
    case SELECT_BUFFER:
        ctlBufSelect( value );
        break;
    case CHANGE_PITCH:
        ctlPitch( normalizedValue );
        break;
    case CHANGE_VOLUME:
        ctlVolume( normalizedValue );
        break;
    }
}

// HELPER METHODS
// Scale MIDI event value to range [0.0, 0.1]
double Controller::normalizeMidiValue( int value ) 
{
    return static_cast<double>(value) / 127.0;
}

// CONTROL METHODS (mostly passthrough, maybe refactor into switch statement)
// Calls seek function on active player
void Controller::ctlSeek( double value )
{
    players->at(active).seek( players->at(active).getDuration() * value );
}

// Calls loop start on player
void Controller::ctlLoopStart( double value )
{
    players->at(active).set_loop_start( players->at(active).getDuration() * value );
}

// Calls loop length on player
void Controller::ctlLoopLength( double value )
{
    double maxLength = 16; // Intervals of 0.063 seconds per MIDI CC value
    players->at(active).set_loop_length( value * maxLength);
}

// Calls loop end on active player
void Controller::ctlLoopEnd( double value )
{
    players->at(active).set_loop_end( players->at(active).getDuration() * value );
}

// Calls speed control on active player
void Controller::ctlSpeed( double value )
{
    double max_speed = 4.0; 
    double speed = value * (max_speed-0.25) + 0.25; // Min speed is 0.25 with audio
    players->at(active).set_rate( speed );
}

// Calls pitch control on active player
void Controller::ctlPitch( double value )
{
    // Map to integer range -12 - 12
    int semitones = static_cast<int>(value * 24) - 12;
    players->at(active).setPitch( semitones );
}

// Calls volume control on active player
void Controller::ctlVolume( double value )
{
    double volume = value * 100;
    players->at(active).setVolume( volume );
}

// Changes currently selected buffer
void Controller::ctlBufSelect( int value )
{
    // Handle Arturia minilab presets (Placeholder)
    // TODO abstraction
//    std::cout << "Received BufferSelect " << value << '\n';
    active = value - 48;
//    std::cout << "Set active buffer to " << value -48 << '\n';
}

