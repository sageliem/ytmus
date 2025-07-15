// ncurses TUI for ytcc2
#include "tui.hpp"
#include <iostream>

// Create TUI app class
TuiApp::TuiApp() {
  for (int i = 0; i < 8; i++) {
    windows.push_back(std::make_unique<PlayerWindow>(i));
  }
}

// setup(): enter ncurses mode and create windows
void TuiApp::setup() {
  initscr();
  if (stdscr == NULL) {
    std::cout << "Failed to initialize ncurses";
    return;
  }
  getmaxyx(stdscr, height, width);
  for (int i = 0; i < 8; i++) {
    windows[i]->setPos(0, (height / 8) * i, width, height / 8);
  }
}

// TODO
void TuiApp::setActiveWindow(const int &id) {
  for (const auto &window : windows)
    window->setActive(false);
  windows[id]->setActive(true);
}

std::unique_ptr<PlayerWindow> &TuiApp::getWindow(const int &id) {
  return windows[id];
}

// Called each frame, updates each window and then displays to screen
void TuiApp::update() {
  for (std::unique_ptr<PlayerWindow> &window : windows) {
    window->update();
  }

  doupdate(); // Display to screen
}

// Closes windows, main screen, and exits ncurses mode
void TuiApp::close() {
  for (std::unique_ptr<PlayerWindow> &window : windows) {
    window->close();
  }
  delwin(stdscr);
  endwin();
}

//
// PlayerWindow class
//
PlayerWindow::PlayerWindow(const int id) : id{id} {}

void PlayerWindow::setPos(const int x, const int y, const int w, const int h) {
  height = h;
  width = w;

  this->y = y;
  this->x = x;

  // Initialize window element
  win = newwin(height, width, y, x);
}

// Update window each frame, displaying player info if available
void PlayerWindow::update() {
  werase(win);

  if (active)
    box(win, '0', '0');
  else
    box(win, 0, 0);

  mvwprintw(win, 0, 2, "| %d | ", id);

  // Display player info if player is loaded
  wprintw(win, "%s", name.c_str());

  wprintw(win, " | %.2f |", timePos);

  mvwprintw(win, 2, 2, "loop[%.2f, %.2f]", loopStart, loopEnd);
  mvwprintw(win, 3, 2, "speed[%.2f]", speed);
  mvwprintw(win, 4, 2, "pitch[%.2f]", pitch);
  mvwprintw(win, 5, 2, "volume[%.2f]", volume);

  // Print contents to virtual screen
  wnoutrefresh(win);
}

void PlayerWindow::close() { delwin(win); }

void PlayerWindow::setName(const std::string &name) { this->name = name; }
void PlayerWindow::setTimePos(const double &pos) { this->timePos = pos; }
void PlayerWindow::setLoopStart(const double &start) {
  this->loopStart = start;
}
void PlayerWindow::setLoopLength(const double &loopLength) {
  loopEnd = loopEnd + loopLength;
}
void PlayerWindow::setSpeed(const double &speed) { this->speed = speed; }
void PlayerWindow::setPitch(const double &pitch) { this->pitch = pitch; }
void PlayerWindow::setVolume(const double &volume) { this->volume = volume; }
void PlayerWindow::setActive(bool opt) { this->active = opt; }
