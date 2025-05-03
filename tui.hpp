#pragma once

#include <ncurses.h>
#include "player.hpp"
#include <string>

class TuiApp 
{
    int width;
    int height;
public:
    TuiApp();
    void setup();
    void update();
    void close();
};

class BufferWindow
{
    Player *p; 
    const char id;
    std::string name;
    bool active;
public:
    BufferWindow(const char id);
    void setup(Player *p_init);
    void update();
    void close();
};
