// ncurses TUI for ytcc2

#include "tui.hpp"

// Create TUI app class
TuiApp::TuiApp() : width{}, height{} {
  for (int i = 0; i < 8; ++i) {
    windows[i] = std::make_unique<PlayerWindow>(i);
  }
}

// setup(): enter ncurses mode and create windows
void TuiApp::setup() {
  initscr();
  getmaxyx(stdscr, height, width);
  for (int i = 0; i < 8; ++i) {
    windows[i]->setPos(height / 8, width, height / 8 * i, 0);
  }
}

// TODO
void TuiApp::setActiveWindow(const int &id) {}

const std::unique_ptr<PlayerWindow> &TuiApp::getWindow(const int &id) {
  return windows[id];
}

// Called each frame, updates each window and then displays to screen
void TuiApp::update() {
  for (std::unique_ptr<PlayerWindow> &window : windows) {
    window->update();
  }

  doupdate();
}

// Closes windows, main screen, and exits ncurses mode
void TuiApp::close() {
  for (std::unique_ptr<PlayerWindow> &window : windows) {
    window->close();
  }
  delwin(stdscr);
  endwin();
}

// Window class created for each buffer
// BufferWindow::BufferWindow(const char id_init)
//     : win{nullptr}, p{nullptr}, id{id_init}, name{""} {}
//
// Create window displaying player info
void PlayerWindow::setPos(const int h_init, const int w_init, const int y_init,
                          const int x_init) {
  height = h_init;
  width = w_init;

  y = y_init;
  x = x_init;

  // Initialize window element
  win = newwin(height, width, y, x);
}

// Redirect buffer window to a different player
// void BufferWindow::load_player(std::unique_ptr<Player> &p_new) {
//   p = p_new;
//   if (p) {
//     name = p->getTitle();
//   }
// }

// Update window each frame, displaying player info if available
void PlayerWindow::update() {
  werase(win);

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

// Close buffer window element
void PlayerWindow::close() { delwin(win); }
