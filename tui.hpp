#pragma once

#include <ncurses.h>
#include "player.hpp"
#include <string>
#include <array>

class BufferWindow
{
    WINDOW *win;
    Player *p; 
    const char id;
    std::string name;
    bool active;
    int height;
    int width;
    int x;
    int y;
public:
    BufferWindow(const char id);
    void setup(Player *p_init, const int h_init, const int w_init, const int y_init, const int x_init);
    void load_player(Player *p_new);
    void update();
    void close();
};

class TuiApp 
{
    int width;
    int height;
    std::array<std::unique_ptr<BufferWindow>, 8> windows;
public:
    TuiApp();
    void setup();
    void update();
    void close();
    void load_buffer(const int id, Player *p);
};


