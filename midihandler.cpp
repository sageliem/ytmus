#include <iostream>

#include "midihandler.hpp"
#include "types.hpp"

#include "controller.hpp"

// Helper for midiCallback
// Hardcoded to Arturia Minilab Mkii
controlEvent midiCCEventType( int midiCCNumber )
{
    controlEvent controlType;
    switch( midiCCNumber )
    {
    // Knob 8 SEEK
    case 75: 
        controlType = SEEK;
        break;
    // Knob 7 SPEED
    case 73:
        controlType = CHANGE_SPEED;
        break;
    // Knob 2 LOOP_START
    case 74:
        controlType = LOOP_START;
        break;
    // Knob 3 LOOP_LENGTH
    case 71:
        controlType = LOOP_LENGTH;
        break;
    // Knob 6 PITCH
    case 93:
        controlType = CHANGE_PITCH;
        break;
    // Knob 10 VOLUME
    case 18:
        controlType = CHANGE_VOLUME;
        break;
    }

    return controlType;
}


// Called each MIDI event
// TODO refactor into MidiHandler class
void midiCallback(double deltatime, std::vector<unsigned char>* message, void* userData)
{
    // Filter out nonstandard message types
    if ( message->size() != 3 ) return; 

    // Output values
    controlEvent controlType { SELECT_BUFFER };
    int controlValue { 0 };

    // TODO clean up types
    unsigned int status = static_cast<unsigned int>( (*message)[0] ); // Message type
    unsigned int status_shift = status >> 4;
    int midiNumber  = (*message)[1];    // Note/CC number
    int midiValue   = (*message)[2];    // Note/CC value

    switch ( status_shift )
    {
    // CC message
    case 0xB:
//           std::cout << "CC Input\n";
        controlType = midiCCEventType( midiNumber );
        controlValue = midiValue;

    // NOTE_ON
    case 0x9:
        if ( midiNumber>=48 && midiNumber<=56 )
        {
            controlType = SELECT_BUFFER;
            controlValue = midiNumber;
        }
//            std::cout << "NoteOn number " << number << "\n";
        break;

    // NOTE_OFF
    case 0x8:
//      std::cout << "Note Off\n";
        break;
    }

//    std::cout << "Sending data to Controller\n";
    static_cast<Controller*>( userData ) -> handleControlEvent(controlType, controlValue);
}


// Initialize midi controller, select port
void MidiHandler::setup(Controller* controller)
{
    // Get port count
    unsigned int nPorts { midi.getPortCount() };
    std::cout << "There are " << nPorts << " MIDI input sources available.\n";

    std::string portName; // Stores current port name

    // List available ports
    for ( unsigned int i=0; i<nPorts; i++ )
    {
        try
        {
            portName = midi.getPortName( i );
        }
        catch ( RtMidiError &error )
        {
            error.printMessage();
            return;
        }

        std::cout << "  Input Port #" << i << ": " << portName << '\n';
    }

    // Get port selection from user
    unsigned int portNum;
    std::cout << "Select a port: ";
    std::cin >> portNum;

    // Attempt to open selected port
    try
    {
        midi.openPort( portNum );
    }
    catch ( RtMidiError &err )
    {
        err.printMessage();
        return;
    }

    std::cout << "Listening on port " << midi.getPortName() << '\n';

    // Ignore special messages (reduces number of callbacks)
    midi.ignoreTypes(true, true, true);

    // Set callback function, pass pointer to controller object
    midi.setCallback( &midiCallback, controller );
}

// Placeholder, not needed currently
void MidiHandler::update()
{
    return;
}
