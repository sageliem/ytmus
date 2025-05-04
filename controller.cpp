#include "controller.hpp"
#include "player.hpp"

#include <iostream>

Controller::Controller(std::array<Player, 8>* players)
  : players { players },
    active { 0 }
{}

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

// Helper function to get ratio from MIDI integer value (0-127)
double Controller::normalizeMidiValue( int value ) 
{
    return static_cast<double>(value) / 127.0;
}

void Controller::ctlSeek( double value )
{
    players->at(active).seek( players->at(active).getDuration() * value );
}

void Controller::ctlLoopStart( double value )
{
    players->at(active).set_loop_start( players->at(active).getDuration() * value );
}

void Controller::ctlLoopLength( double value )
{
    double maxLength = 16; // Intervals of 0.063 seconds per MIDI CC value
    players->at(active).set_loop_length( value * maxLength);
}

void Controller::ctlLoopEnd( double value )
{
    players->at(active).set_loop_end( players->at(active).getDuration() * value );
}

void Controller::ctlSpeed( double value )
{
    double max_speed = 4.0; 
    double speed = value * (max_speed-0.25) + 0.25; // Min speed is 0.25 with audio
    players->at(active).set_rate( speed );
}

void Controller::ctlPitch( double value )
{
    // Map to integer range -12 - 12
    int semitones = static_cast<int>(value * 24) - 12;
    players->at(active).setPitch( semitones );
}

void Controller::ctlVolume( double value )
{
    double volume = value * 100;
    players->at(active).setVolume( volume );
    std::cout << "Controller worked\n"
}

void Controller::ctlBufSelect( int value )
{
    // Handle Arturia minilab presets (Placeholder)
    // TODO abstraction
//    std::cout << "Received BufferSelect " << value << '\n';
    active = value - 48;
//    std::cout << "Set active buffer to " << value -48 << '\n';
}


