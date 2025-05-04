#include <iostream>

#include "midictl.hpp"
#include "types.hpp"

#include "controller.hpp"

void midiCallback(double deltatime, std::vector<unsigned char>* message, void* userData)
{
    // Output values
    controlEvent controlType; // 
    int controlValue;

    if ( message->size() == 3 )
    {
        // TODO clean up types
        unsigned int status = static_cast<unsigned int>( (*message)[0] ); // Message type
        unsigned int status_shift = status >> 4;
        int number  = (*message)[1];    // Note/CC number
        int value   = (*message)[2];    // Note/CC value

        switch ( status_shift )
        {
        // CC message
        case 0xB:
 //           std::cout << "CC Input\n";
            switch( number )
            {
            // Knob 8 SEEK
            case 75: 
                controlType = SEEK;
                controlValue = value;
                break;
            // Knob 7 SPEED
            case 73:
                controlType = CHANGE_SPEED;
                controlValue = value;
                break;
            // Knob 2 LOOP_START
            case 74:
                controlType = LOOP_START;
                controlValue = value;
                break;
            // Knob 3 LOOP_END
            case 71:
                controlType = LOOP_END;
                controlValue = value;
                break;
            // Knob 6 PITCH
            case 93:
                controlType = CHANGE_PITCH;
                controlValue = value;
                break;
            // Knob 10 VOLUME
            case 18:
                controlType = CHANGE_VOLUME;
                controlValue = value;
                break;
            }


        // NOTE_ON
        case 0x9:
            if ( number>=48 && number<=56 )
            {
                controlType = SELECT_BUFFER;
                controlValue = number;
            }
//            std::cout << "NoteOn number " << number << "\n";
            break;

        // NOTE_OFF
        case 0x8:
//            std::cout << "Note Off\n";
            break;

        }
    }
    
//    std::cout << "Sending data to Controller\n";
    static_cast<Controller*>( userData ) -> handleControlEvent(controlType, controlValue);
}



void MidiController::setup(Controller* controller)
{
    unsigned int nPorts { midi.getPortCount() };
    std::cout << "There are " << nPorts << " MIDI input sources available.\n";
    std::string portName;

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

    unsigned int portNum;
    std::cout << "Select a port: ";
    std::cin >> portNum;

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

    midi.ignoreTypes(true, true, true);

    // Pass pointer to controller object
    midi.setCallback(&midiCallback, controller);

}

void MidiController::update()
{
    return;
}
