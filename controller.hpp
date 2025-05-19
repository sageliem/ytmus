// Controller.hpp
//
#pragma once

#include "types.hpp"
#include <array>
 
class Player; // Forward declaration of player

class Controller
{
    std::array<Player, 8>* players;
    int active;

    double normalizeMidiValue( int value ); // Normalize MIDI event value 0.0-1.0
                                            
public:
    void ctlSeek( double value );
    void ctlLoopStart( double value );
    void ctlLoopEnd( double value );
    void ctlLoopLength( double value );
    void ctlSpeed( double value );
    void ctlBufSelect( int value );
    void ctlPitch( double value );
    void ctlVolume( double value );


    Controller(std::array<Player, 8>* players);
    void handleControlEvent(controlEvent type, int value);
};

