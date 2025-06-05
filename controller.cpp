#include "controller.hpp"
#include "player.hpp"

#include <chrono>
#include <iostream>
#include <math.h>
#include <mutex>

bool operator>( const Event& ls, const Event& rs )
{
    return ls.when > rs.when;
}

// Create an instance of Controller holding pointer to array of Players
Controller::Controller(std::array<Player, 8>* players)
  : players { players },
    active { 0 }
{
    start = std::chrono::high_resolution_clock::now();
}


// Called by main()
void Controller::update()
{
//    sched_mutex.lock();
    time = std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::high_resolution_clock::now() - start );

    if (sched.empty()) return;

    Event soonest = sched.top();
//    std::cout << "Time: " << time.count() << "  soonest event: " << soonest.when.count() << '\n';
    if ( soonest.when <= time )
    {
        std::cout << "Running control event with scheduled time " << soonest.when.count() << "\n";
        doControlEvent( soonest.eventType, soonest.controlValue );
        sched.pop();
    }

//    sched_mutex.unlock();
}

void Controller::lockQueue() 
{
    sched_mutex.lock();
}

void Controller::unlockQueue()
{
    sched_mutex.unlock();
}

// Handles a control event. Called by MidiController (rename to MidiHandler). TODO implement other Handler objects (OSC, keyboard control, etc)
void Controller::handleControlEvent( controlEvent eventType, double controlValue, int delayMillis )
{
    time = std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::high_resolution_clock::now() - start );
    sched.push( Event{ time + std::chrono::milliseconds( delayMillis ), eventType, controlValue } );
}


// Perform event (called by update)
void Controller::doControlEvent( controlEvent eventType, double controlValue)
{
    // Placeholder code to fix out of range issues TODO
    //if (eventType == SELECT_BUFFER && controlValue == 0) return; 

    switch ( eventType )
    {
    case CHANGE_SPEED:
        ctlSpeed( controlValue );
        break;
    case LOOP_START:
        ctlLoopStart( controlValue );
        break;
    case LOOP_END:
        ctlLoopEnd( controlValue );
        break;
    case LOOP_LENGTH:
        ctlLoopLength( controlValue );
        break;
    case SEEK:
        ctlSeek( controlValue );
        break;
    case SELECT_BUFFER:
        ctlBufSelect( controlValue );
        break;
    case CHANGE_PITCH:
        ctlPitch( controlValue );
        break;
    case CHANGE_VOLUME:
        ctlVolume( controlValue );
        break;
    }
}

// CONTROL METHODS (mostly passthrough, maybe refactor into switch statement)
// Calls seek function on active player
void Controller::ctlSeek( double controlValue )
{
    std::cout << "ctlSeek: " << controlValue << '\n';
//    players->at(active).seek( players->at(active).getDuration() * controlValue );
    players->at(active).seek( controlValue );
}

// Calls loop start on player
void Controller::ctlLoopStart( double controlValue )
{
    players->at(active).set_loop_start( players->at(active).getDuration() * controlValue );
}

// Calls loop length on player
void Controller::ctlLoopLength( double controlValue )
{
    double maxLength = 16; // Intervals of 0.063 seconds per MIDI CC controlValue
    players->at(active).set_loop_length( controlValue * maxLength);
}

// Calls loop end on active player
void Controller::ctlLoopEnd( double controlValue )
{
    players->at(active).set_loop_end( players->at(active).getDuration() * controlValue );
}

// Calls speed control on active player
void Controller::ctlSpeed( double controlValue )
{
    double max_speed = 4.0; 
    double speed = controlValue * (max_speed-0.25) + 0.25; // Min speed is 0.25 with audio
    players->at(active).set_rate( speed );
}

// Calls pitch control on active player
void Controller::ctlPitch( double controlValue )
{
    // Map to integer range -12 - 12
    int semitones = static_cast<int>(controlValue * 24) - 12;
    players->at(active).setPitch( semitones );
}

// Calls volume control on active player
void Controller::ctlVolume( double controlValue )
{
    double volume = controlValue * 100;
    players->at(active).setVolume( volume );
}

// Changes currently selected buffer
void Controller::ctlBufSelect( int controlValue )
{
    // Handle Arturia minilab presets (Placeholder)
    // TODO abstraction
    std::cout << "BufferSelect " << controlValue << '\n';
    active = static_cast<int>(std::floor(controlValue));
//    std::cout << "Set active buffer to " << controlValue -48 << '\n';
}

