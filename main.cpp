#include <iostream>
#include <rtmidi/RtMidi.h>
#include <jack/jack.h>

#include "player.hpp"
#include "controller.hpp"
#include "midictl.hpp"
#include "tui.hpp"

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

    std::array<Player, 8> players {};
    
    Controller controller ( &players );

    bool showUi = true;


//    jack_client_t* jack = jack_client_open("ytcc", JackNullOption, nullptr);
//    jack_set_process_callback(jack, )


    // Set up MIDI control
    MidiController midi;
    midi.setup( &controller );

    // Initialize players
    players[0].load("https://www.youtube.com/watch?v=-ASxhgeVBpo&list=PLd6fsLXcfffBeejKYqxp_vS8sipCg6NEa&index=14"); // ECM trumpet
//    p.load("https://www.youtube.com/watch?v=DkLEZ0-l70U&list=LL&index=59"); // Peace
//    p.load("https://www.youtube.com/watch?v=5Dkf6UUfwQk&list=PLuoCvADUeA_KmRoDuAu4zWRksB8lnpkKC&index=20"); // Neanic
//    p.load("https://www.youtube.com/watch?v=CxPSbe8xNyQ"); // Grant Gordy
//    p.load("./handpan.wav");


    // Set up TUI
    TuiApp ui;

    if (showUi) {
        ui.setup();
        ui.load_buffer(0, &(players[0]));
    }


    // Main loop
    while (1) {
        for (Player& p : players) {
            try 
            {
                p.update();
            } 
            catch ( std::exception& err )
            {
                std::cerr << "Exception while updating players: " << err.what() << '\n';
                if (showUi) ui.close();
                return 1;
            }
            catch (...)
            {
                if (showUi) ui.close();
                return 1;
            }
        }
//        std::cout << "Current time: " << p.getCurrentPos() << " Loop: [" << p.getLoopStart() << ", " << p.getLoopEnd() << "]\n";

        if (showUi) ui.update();
    }

    if (showUi) ui.close();

    return 0;
}
