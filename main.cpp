#include <iostream>
#include <rtmidi/RtMidi.h>

#include "player.hpp"

void set_loopstart(Player *p, double loopstart) {
    double dur = p->getDuration();
    p->set_loop_start(loopstart * dur);
}

void set_loopend(Player *p, double loopend) {
    double dur = p->getDuration();
    p->set_loop_end(loopend * dur);
}

// called when MIDI data is received
void midiCallback(double deltatime, std::vector<unsigned char>* message, void* userData) {
    // MIDI CC message: status byte, control number, control value
    if (message->size() == 3 && (*message)[0] == 0xB0) {  // Check if it's a CC message (0xB0 is for CC)
        int controlNumber = (*message)[1];  // MIDI CC number (e.g., 7 for volume)
        int controlValue = (*message)[2];   // Value (0-127)

        if (controlNumber == 33) {  // Loop Start control
            std::cout << "Received CC 33 with value" << controlValue << "\n";
            float startctl = controlValue / 127.0f;  // Normalize to 0.0 - 1.0
            set_loopstart((Player*)userData, startctl);
        } else if (controlNumber == 34) {
            std::cout << "Received CC 34 value " << controlValue << "\n";
            double endctl = controlValue / 127.0f;
            set_loopend((Player*)userData, endctl);
        }
    }
}

int main() {

    Player p {};

    RtMidiIn midi;

    unsigned int n_ports{ midi.getPortCount() }; 
    if (n_ports == 0) {
        std::cout << "No ports available\n";
        return 1;
    }

    midi.openPort(0);
    midi.setCallback(&midiCallback, &p);
    std::cout << "Listening on port " << midi.getPortName() << '\n';

    //midi.ignoreTypes(false, false, false);


    p.load("https://www.youtube.com/watch?v=CxPSbe8xNyQ");
//    p.load("./handpan.wav");

    while (1) {
        p.update();
        std::cout << "Current time: " << p.getCurrentPos() << " Loop: [" << p.getLoopStart() << ", " << p.getLoopEnd() << "]\n";
    }

    return 0;
}
