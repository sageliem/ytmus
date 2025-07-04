#include <iostream>
// #include <jack/jack.h>

#include "controller.hpp"
#include "midihandler.hpp"
#include "oschandler.hpp"
#include "player.hpp"
#include "tui.hpp"

/*
// JACK callback
int process(jack_nframes_t nframes, void* players) {
    for (int i=0; i<8; i++) {
        float* buffer = (float*)jack_port_get_buffer(players[i].output_port,
nframes); players[i].fill_audio(buffer, nframes);
    }
    return 0;
}
*/

int main() {

  // Testing for OscHandler
  OscHandler osc(5050);

  std::vector<std::unique_ptr<Player>> players{};

  Controller controller(players);

  // bool showUi = false;

  //    jack_client_t* jack = jack_client_open("ytcc", JackNullOption, nullptr);
  //    jack_set_process_callback(jack, )

  // Set up MIDI control
  MidiHandler midi;
  midi.setup(&controller);
  midi.setRelativeKnobs(true);

  players.push_back(std::make_unique<Player>());
  players.push_back(std::make_unique<Player>());

  // Initialize players
  players[0]->load("https://www.youtube.com/watch?v=-ASxhgeVBpo"); // ECM
  //    trumpet
  //    players[1].load("https://youtu.be/4CaSXEEqKBU?si=HU0oqygxxOOSojvL");
  //    p.load("https://www.youtube.com/watch?v=DkLEZ0-l70U&list=LL&index=59");
  //    // Peace
  //    p.load("https://www.youtube.com/watch?v=5Dkf6UUfwQk&list=PLuoCvADUeA_KmRoDuAu4zWRksB8lnpkKC&index=20");
  //    // Neanic p.load("https://www.youtube.com/watch?v=CxPSbe8xNyQ"); //
  //    Grant Gordy players[0].load("./handpan.wav");
  //    players[0]->load("https://www.youtube.com/watch?v=oxQmec6gGV0");
  //  players[1]->load("https://www.youtube.com/shorts/HzfMYiGj7M4");
  // players[2].load("https://www.youtube.com/watch?v=FY1CwzIjoJQ");
  //    players[3].load("https://www.youtube.com/watch?v=ngfBSBoYzUY");

  osc.init(&controller);
  std::cout << "Initialized OSC listener\n";

  // Set up TUI
  // TuiApp ui;
  //
  // if (showUi) {
  //   ui.setup();
  //   ui.load_buffer(0, &(players[0]));
  //   ui.load_buffer(1, &(players[1]));
  //
  //   ui.load_buffer(2, &(players[2]));
  //   ui.load_buffer(3, &(players[3]));
  // } else {
  //   std::cout << "UI Disabled\n";
  // }
  //
  // Main loop
  while (1) {

    controller.update();
    for (std::unique_ptr<Player> &p : players) {
      try {
        p->update();
      } catch (std::exception &err) {
        std::cerr << "Exception while updating players: " << err.what() << '\n';
        // if (showUi)
        //   ui.close();
        // return 1;
      } catch (...) {
        // if (showUi)
        // ui.close();
        return 1;
      }
    }

    // Check schedule for events

    //        std::cout << "Current time: " << p.getCurrentPos() << " Loop: ["
    //        << p.getLoopStart() << ", " << p.getLoopEnd() << "]\n";

    // if (showUi)
    // ui.update();
  }

  // if (showUi)
  // ui.close();

  return 0;
}
