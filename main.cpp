#include <fstream>
#include <iostream>

#include "controller.hpp"
#include "midihandler.hpp"
#include "oschandler.hpp"
#include "player.hpp"
#include "tui.hpp"

int main() {

  // Testing for OscHandler
  OscHandler osc(5050);

  std::vector<std::unique_ptr<Player>> players{};
  auto ui = std::make_unique<TuiApp>();

  bool showUi = true;

  // Set up TUI
  if (showUi) {
    ui->setup();
  } else {
    ui = nullptr;
    std::cout << "UI Disabled\n";
  }

  Controller controller(players, ui);

  // Set up MIDI control
  MidiHandler midi(&controller);
  midi.setup();
  midi.setRelativeKnobs(true);

  // Initialize players from vids.txt
  std::ifstream vid_urls("vids.txt");
  if (!vid_urls.is_open()) {
    std::cerr << "Failed to open file\n";
    return 1;
  }

  std::string url;
  int counter = 0;
  while (std::getline(vid_urls, url)) {
    players.push_back(std::make_unique<Player>());
    players[counter]->load(url);
    counter++;
  }

  osc.init(&controller);
  std::cout << "Initialized OSC listener\n";

  // Main loop
  while (1) {

    controller.update();
    for (std::unique_ptr<Player> &p : players) {
      try {
        p->update();
      } catch (std::exception &err) {
        std::cerr << "Exception while updating players: " << err.what() << '\n';
        if (showUi)
          ui->close();
        return 1;
      } catch (...) {
        if (showUi)
          ui->close();
        return 1;
      }
    }

    if (showUi)
      ui->update();
  }

  if (showUi)
    ui->close();

  return 0;
}
