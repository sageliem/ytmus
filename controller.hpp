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

    void ctlSeek(int value);
    void ctlLoopStart(int value);
    void ctlLoopEnd(int value);
    void ctlSpeed(int value);
    void ctlBufSelect( int value );
    void ctlPitch( int value );
    void ctlVolume( int value );

public:
    Controller(std::array<Player, 8>* players);
    void handleControlEvent(controlEvent type, int value);
};

