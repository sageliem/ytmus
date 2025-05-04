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

    switch ( type )
    {
    case CHANGE_SPEED:
        ctlSpeed( value );
        break;
    case LOOP_START:
        ctlLoopStart( value );
        break;
    case LOOP_END:
        ctlLoopEnd( value );
        break;
    case SEEK:
        ctlSeek( value );
        break;
    case SELECT_BUFFER:
        ctlBufSelect( value );
        break;
    case CHANGE_PITCH:
        ctlPitch( value );
        break;
    case CHANGE_VOLUME:
        ctlVolume( value );
        break;
    }
}

void Controller::ctlSeek( int value )
{
    players->at(active).seek( players->at(active).getDuration() / 127.0 * value );
}

void Controller::ctlLoopStart( int value )
{
    players->at(active).set_loop_start( players->at(active).getDuration() / 127.0 * value );

}

void Controller::ctlLoopEnd( int value )
{
    players->at(active).set_loop_end( players->at(active).getDuration() / 127.0 * value );

}

void Controller::ctlSpeed( int value )
{
    double max_speed = 4.0;
    // Min speed 0.25 with audio
    players->at(active).set_rate( value / 127.0 * (max_speed-0.25) + 0.25 );

}

void Controller::ctlPitch( int value )
{
    // Map to integer range -12 - 12
    double normalized = static_cast<double>(value) / 127.0;
    int semitones = static_cast<int>(normalized * 24) - 12;
    players->at(active).setPitch( semitones );
}

void Controller::ctlVolume( int value )
{
    double volume = static_cast<double>(value) / 127.0 * 100;
    players->at(active).setVolume( volume );
}

void Controller::ctlBufSelect( int value )
{
    // Handle Arturia minilab presets (Placeholder)
    // TODO abstraction
//    std::cout << "Received BufferSelect " << value << '\n';
    active = value - 48;
//    std::cout << "Set active buffer to " << value -48 << '\n';
}


