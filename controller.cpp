#include "controller.hpp"
#include "player.hpp"

Controller::Controller(std::array<Player, 8>* players)
  : players { players },
    active { 0 }
{}

void Controller::handleControlEvent(controlEvent type, int value)
{
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

void Controller::ctlBufSelect( int value )
{
    // Handle Arturia minilab presets (Placeholder)
    // TODO abstraction
    active = value - 28;
}


