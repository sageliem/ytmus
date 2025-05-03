#include <iostream>
#include <rtmidi/RtMidi.h>
#include <jack/jack.h>

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
    if (message->size() == 3) {
        switch (static_cast<unsigned int>((*message)[0])) {
        case 0xB0: // CC message
            int cc_number = (*message)[1];  // MIDI CC number (e.g., 7 for volume)
            int cc_value = (*message)[2];   // Value (0-127)

            switch (cc_number) {
            case 75: // Knob 8, seek
                ((Player*)userData)->seek(cc_value / 127.0 * ((Player*)userData)->getDuration());
            case 73: // Knob 7, speed
                ((Player*)userData)->set_rate(cc_value / 127.0 * 2);
            case 74: // Knob 2, set loop start
                set_loopstart((Player*)userData, cc_value / 127.0);
                break;
            case 71: // Knob 3, set loop end
                set_loopend((Player*)userData, cc_value / 127.0);
                break;
            }
        }
    }
}

/*
// JACK callback
int process(jack_nframes_t nframes, void* players) {
    for (int i=0; i<8; i++) {
        float* buffer = (float*)jack_port_get_buffer(players[i].output_port, nframes);
        players[i].fill_audio(buffer, nframes);
    }
    return 0;
}
*/

int main() {

    Player p {};

    RtMidiIn midi;

//    jack_client_t* jack = jack_client_open("ytcc", JackNullOption, nullptr);
//    jack_set_process_callback(jack, )

    unsigned int n_ports{ midi.getPortCount() }; 
    if (n_ports == 0) {
        std::cout << "No ports available\n";
        return 1;
    }
        // Check inputs.
    unsigned int nPorts = midi.getPortCount();
    std::cout << "\nThere are " << nPorts << " MIDI input sources available.\n";
    std::string portName;
    for ( unsigned int i=0; i<nPorts; i++ ) {
        try {
          portName = midi.getPortName(i);
        }
        catch ( RtMidiError &error ) {
          error.printMessage();
          return 1;
        }
    std::cout << "  Input Port #" << i+1 << ": " << portName << '\n';
    }

    midi.openPort(4);
    midi.setCallback(&midiCallback, &p);
    std::cout << "Listening on port " << midi.getPortName() << '\n';

    //midi.ignoreTypes(false, false, false);

    p.load("https://www.youtube.com/watch?v=-ASxhgeVBpo&list=PLd6fsLXcfffBeejKYqxp_vS8sipCg6NEa&index=14"); // ECM
                                                                                                           // trumpet
//    p.load("https://www.youtube.com/watch?v=DkLEZ0-l70U&list=LL&index=59"); // Peace
                                                                           // headlock
//    p.load("https://www.youtube.com/watch?v=5Dkf6UUfwQk&list=PLuoCvADUeA_KmRoDuAu4zWRksB8lnpkKC&index=20"); // Neanic
//    p.load("https://www.youtube.com/watch?v=CxPSbe8xNyQ"); // Grant Gordy
//    p.load("./handpan.wav");

    while (1) {
        p.update();
        std::cout << "Current time: " << p.getCurrentPos() << " Loop: [" << p.getLoopStart() << ", " << p.getLoopEnd() << "]\n";
    }

    return 0;
}
