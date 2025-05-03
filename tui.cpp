// ncurses TUI for ytcc2

#include "tui.hpp"

// Main TUI app
TuiApp::TuiApp()
  : width{},
    height{}
{
    for (int i=0; i<8; ++i) {
        windows[i] = std::make_unique<BufferWindow>(i);
    }
}


void TuiApp::setup() 
{
    initscr();
    getmaxyx(stdscr, height, width);
    for (int i=0; i<8; ++i) {
        windows[i]->setup(nullptr, height/8, width, height/8*i, 0);
    }
}

void TuiApp::load_buffer( const int id, Player *p )
{
    windows[id]->load_player(p);
}

void TuiApp::update()
{
//    werase(stdscr);
    //box(stdscr, 0, 0);
    
    for (std::unique_ptr<BufferWindow> &window : windows)
    {
        window->update();
    }


    
    doupdate();
}

void TuiApp::close()
{
    for (std::unique_ptr<BufferWindow> &window : windows)
    {
        window->close();
    }
    delwin(stdscr);
    endwin();
}



// Window class for each buffer
BufferWindow::BufferWindow(const char id_init)
  : p { nullptr },
    win { nullptr },
    id { id_init },
    name { "" },
    active { false }
{}

// Create window
void BufferWindow::setup(Player *p_init, const int h_init, const int w_init, const int y_init, const int x_init)
{
    height = h_init;
    width = w_init;

    y = y_init;
    x = x_init;

    // Initialize window element
    win = newwin(height, width, y, x);

    p = p_init;
    if (p) {
        name = p->getPath();
    }
}

void BufferWindow::load_player(Player *p_new)
{
    p = p_new;
    if (p)
    {
        name = p->getPath();
    }
}

// Update window each frame
void BufferWindow::update()
{
    werase(win);

    box(win, 0, 0);

    mvwprintw(win, 0, 2, "| %d | ", id);

    if (p)
    {
        wprintw(win, name.c_str());

        double timepos = p->getCurrentPos();
        wprintw(win, " | %.2f |", timepos);

        mvwprintw( win, 2, 2, "loop[%.2f, %.2f]", p->getLoopStart(), p->getLoopEnd() );
        mvwprintw( win, 3, 2, "speed[%.2f]", p->getSpeed() );
    }

    wnoutrefresh(win);
}

// Close a buffer window element
void BufferWindow::close()
{
    delwin(win);
}
