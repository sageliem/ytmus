// Controller.hpp
//
#pragma once

#include "types.hpp"
#include <array>
#include <chrono>
#include <queue>
#include <mutex>

class Player; // Forward declaration of player


// Event class for schedule queue
typedef struct Event 
{
    std::chrono::milliseconds when;
    controlEvent eventType;
    double controlValue;
} Event;



class Controller
{
    std::array<Player, 8>* players;
    int active;
    std::chrono::time_point< std::chrono::high_resolution_clock > start;
    std::chrono::milliseconds time;
    std::mutex sched_mutex;
    std::unique_lock<std::mutex> sched_lock;

    double normalizeMidiValue( int value ); // Normalize MIDI event value 0.0-1.0
                                            //

    std::priority_queue< Event, std::vector<Event>, std::greater<Event> > sched;


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

    void handleControlEvent( controlEvent type, double value, int delayMillis );

    void doControlEvent( controlEvent controlType, double controlValue);
    void lockQueue();
    void unlockQueue();

    void update();
};

