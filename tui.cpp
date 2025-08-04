// ncurses TUI for ytcc2
#include "tui.hpp"
#include <iostream>

// Create TUI app class
TuiApp::TuiApp(std::vector<std::unique_ptr<Player>> &players)
    : players{players} {}

// setup(): enter ncurses mode and create windows
void TuiApp::setup() {
  initscr();
  if (stdscr == NULL) {
    std::cout << "Failed to initialize ncurses";
    return;
  }
  getmaxyx(stdscr, height, width);
}

// TODO
void TuiApp::setActiveWindow(const int &id) {}

// Called each frame, updates each window and then displays to screen
void TuiApp::update() {
  int counter = 0;
  for (std::unique_ptr<Player> &player : players) {
    mvwprintw(stdscr, counter * 2, 2, "| %d | ", counter);
    // Display player info if player is loaded
    wprintw(stdscr, "%s", player->getTitle().c_str());

    wprintw(stdscr, " | %.2f |", player->getCurrentPos());

    wprintw(stdscr, "loop[%.2f, %.2f]", player->getLoopStart(),
            player->getLoopEnd());
    wprintw(stdscr, "speed[%.2f]", player->getSpeed());
    wprintw(stdscr, "pitch[%.2f]", player->getPitch());
    wprintw(stdscr, "volume[%.2f]", player->getVolume());

    // Print contents to virtual screen
    wnoutrefresh(stdscr);
    counter++;
  }
  // for (std::unique_ptr<PlayerWindow> &window : windows) {
  //   window->update();
  // }

  doupdate(); // Display to screen
}

// Closes windows, main screen, and exits ncurses mode
void TuiApp::close() {
  delwin(stdscr);
  endwin();
}
