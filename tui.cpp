// ncurses TUI for ytcc2

#include "tui.hpp"

// Main TUI app
TuiApp::TuiApp()
  : width{},
    height{}
{}


void TuiApp::setup() 
{
    initscr();
    
    getmaxyx(stdscr, height, width);
    box(stdscr, 0, 0);
    wrefresh(stdscr);
}

void TuiApp::update()
{
    werase(stdscr);
    box(stdscr, 0, 0);
    



    
    wrefresh(stdscr);
}

void TuiApp::close()
{
    delwin(stdscr);
    endwin();
}


// Window class for each buffer
BufferWindow::BufferWindow(const char id_init)
  : p { nullptr },
    id { id_init },
    name { "" },
    active { false }
{}

void BufferWindow::setup(Player *p_init)
{
    p = p_init;
    name = p->getPath();
}

void BufferWindow::update()
{

}

void BufferWindow::close()
{

}
