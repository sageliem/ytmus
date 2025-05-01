#pragma once

#include <string>

extern "C" {
    #include <mpv/client.h>
}

// Player struct
class Player {
    mpv_handle *mpv;
    std::string path;
    bool loop;
    double loop_start;
    double loop_end;
    double duration;

public:
    Player();
    ~Player();
    void load(const std::string &url);
    void update();
    void play();
    void pause();
    void set_loop(double start, double end);
    void set_loop_start(double start);
    void set_loop_end(double end);
    void seek(double pos);
    void restart();

    bool isPlaying();
    double getCurrentPos();
    double getDuration();
    double getLoopStart();
    double getLoopEnd();
};

