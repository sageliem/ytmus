// ncurses TUI for ytcc2

#include "tui.hpp"

// Create TUI app class
TuiApp::TuiApp()
  : width{},
    height{}
{
    for (int i=0; i<8; ++i) {
        windows[i] = std::make_unique<BufferWindow>(i);
    }
}

// setup(): enter ncurses mode and create windows
void TuiApp::setup() 
{
    initscr();
    getmaxyx(stdscr, height, width);
    for (int i=0; i<8; ++i) {
        windows[i]->setup(nullptr, height/8, width, height/8*i, 0);
    }
}

// Load buffer window object at index id with Player
void TuiApp::load_buffer( const int id, Player *p )
{
    windows[id]->load_player(p);
}

// Called each frame, updates each window and then displays to screen
void TuiApp::update()
{
    for (std::unique_ptr<BufferWindow> &window : windows)
    {
        window->update();
    }
    
    doupdate();
}

// Closes windows, main screen, and exits ncurses mode
void TuiApp::close()
{
    for (std::unique_ptr<BufferWindow> &window : windows)
    {
        window->close();
    }
    delwin(stdscr);
    endwin();
}


// Window class created for each buffer
BufferWindow::BufferWindow(const char id_init)
  : p { nullptr },
    win { nullptr },
    id { id_init },
    name { "" },
    active { false }
{}

// Create window displaying player info
void BufferWindow::setup(Player *p_init, const int h_init, const int w_init, const int y_init, const int x_init)
{
    height = h_init;
    width = w_init;

    y = y_init;
    x = x_init;

    // Initialize window element
    win = newwin(height, width, y, x);

    load_player(p_init);
}

// Redirect buffer window to a different player
void BufferWindow::load_player(Player *p_new)
{
    p = p_new;
    if (p)
    {
        name = p->getTitle();
    }
}

// Update window each frame, displaying player info if available
void BufferWindow::update()
{
    werase(win);

    box(win, 0, 0);

    mvwprintw(win, 0, 2, "| %d | ", id);

    // Display player info if player is loaded
    if (p)
    {
        wprintw(win, name.c_str());

        double timepos = p->getCurrentPos();
        wprintw(win, " | %.2f |", timepos);

        mvwprintw( win, 2, 2, "loop[%.2f, %.2f]", p->getLoopStart(), p->getLoopEnd() );
        mvwprintw( win, 3, 2, "speed[%.2f]", p->getSpeed() );
        mvwprintw( win, 4, 2, "pitch[%.2f]", p->getPitch() );
        mvwprintw( win, 5, 2, "volume[%.2f]", p->getVolume() );
    }

    // Print contents to virtual screen
    wnoutrefresh(win);
}

// Close buffer window element
void BufferWindow::close()
{
    delwin(win);
}
