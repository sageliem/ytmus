// Player.cpp
// Sage Liem 2025
// mpv wrapper class providing play, pause, loop, and seek functionality

#include "player.hpp"
#include <iostream>
#include <cstdio>

Player::Player() 
  : loop{ false },
    loop_start{},
    loop_end{},
    path{},
    title{},
    duration {},
    port{},
    rate{ 1.0 },
    pitch{ 0.0 }
{
    mpv = mpv_create();
    std::cout << "Created mpv instance\n";

    mpv_initialize(mpv);
    std::cout << "Initialized mpv instance\n";

    // Defaults (no video)
//    mpv_set_option_string(mpv, "vo", "null");
    
    // Video on? Doesn't work
    mpv_set_option_string(mpv, "vo", "gpu-next");
    mpv_set_option_string(mpv, "gpu-api", "vulkan");
    mpv_set_option_string(mpv, "hwdec", "auto-safe");

    // Set my latest yt-dlp version (TODO)
    mpv_set_option_string(mpv, "script-opts", "ytdl_hook-ytdl_path=/Users/sageliem/.pyenv/shims/yt-dlp");

    // May have to replace with a command if using more filters
    // Needs to be named filter @rb to access later
    mpv_set_option_string(mpv, "af", "@rb:rubberband=pitch-scale=1.0");

}

Player::~Player()
{
    mpv_terminate_destroy(mpv);
    std::cout << "Destroyed mpv instance\n";
}

// Load a video
void Player::load(const std::string &url) 
{
    path = url;
    const char *load_cmd[] = {"loadfile", url.c_str(), "replace", NULL};
    std::cout << "Attempting to load file at " << url << '\n';
    mpv_command(mpv, load_cmd);

    // Wait for load successful
    bool loaded = false;
    while (!loaded) {
        mpv_event *event = mpv_wait_event(mpv, 0); // wait for an event (non-blocking)
        if (event->event_id == MPV_EVENT_FILE_LOADED) {
            std::cout << "File loaded: " << path << std::endl;
            loaded = true;
        } 
    }

    mpv_get_property(mpv, "media-title", MPV_FORMAT_STRING, &title);

    // Query duration
    mpv_get_property(mpv, "duration", MPV_FORMAT_DOUBLE, &duration);

    loop_end = duration;
//    std::cout << "Duration: " << duration << '\n';

    // Could add functionality to hide or show video
}


/* TODO
// Initialize JACK port
void Player::init_audio(const std::string &port_name, jack_client_t* client) 
{
    port = jack_port_register(client, port_name.c_str(), JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
}
*/

//TODO Fill audio buffers
void Player::fill_audio(float* buffer, int n_frames)
{
    // TODO
}

void Player::update() 
{
    mpv_event *event = mpv_wait_event(mpv, 0.0); // non-blocking
    if (event->event_id == MPV_EVENT_END_FILE) 
    {
        restart();
    }
    else
    {
        double currentPos = getCurrentPos();
        if (currentPos > loop_end || currentPos < loop_start-0.1) {
            seek(loop_start);
//            std::cout << "Looping\n";
        }
    }
}

// Getters
bool Player::isPlaying() {
    int paused{ 0 };
    mpv_get_property(mpv, "paused", MPV_FORMAT_FLAG, &paused);
    return !paused;
}

std::string Player::getPath()
{
    return path;
}

double Player::getCurrentPos() {
    double currentPos{};
    mpv_get_property(mpv, "time-pos", MPV_FORMAT_DOUBLE, &currentPos);
    return currentPos;
}

double Player::getDuration() {
    return duration;
}

double Player::getLoopStart() {
    return loop_start;
}

double Player::getLoopEnd() {
    return loop_end;
}

double Player::getSpeed()
{
    return rate;
}

double Player::getPitch()
{
    return pitch;
}

double Player::getVolume()
{
    return volume;
}


std::string Player::getTitle()
{
    std::string title_str (title);
    return title_str;
}


// Playback controls
// Play
void Player::play() {
    int paused = 0;
    mpv_set_property(mpv, "pause", MPV_FORMAT_FLAG, &paused);
}

// Pause
void Player::pause() {
    int paused = 1;
    mpv_set_property(mpv, "play", MPV_FORMAT_FLAG, &paused);
}

// Set loop start and end
void Player::set_loop(double start, double end) {
    loop_start = start;
    if (end > start + 0.1) {
        loop_end = end;
    } else {
        loop_end = loop_start + 0.1;
    }
}

// Set loop start
void Player::set_loop_start(double start) {
    if (start < loop_end - 0.1) {
        loop_start = start;
    } else {
        loop_start = loop_end - 0.1;
    }
} 

// Set loop end
void Player::set_loop_end(double end) {
    if (end > loop_start + 0.1) {
        loop_end = end; 
    } else {
        loop_end = loop_start + 0.1;
    }
}

void Player::set_rate(double new_rate) {
    rate = new_rate;
    mpv_set_property(mpv, "speed", MPV_FORMAT_DOUBLE, &new_rate);
}

void Player::setPitch( int semitones )
{
    double semitone_ratio = 1.0595;
    double pitch_ratio = pow( semitone_ratio, static_cast<double>(semitones) );
    pitch = semitones;

    char pitch_ratio_str[64];
    snprintf(pitch_ratio_str, sizeof(pitch_ratio_str), "%.8f", pitch_ratio);

    const char *pitch_cmd[] = { "af-command", "rb", "set-pitch", pitch_ratio_str, NULL };

    mpv_command( mpv, pitch_cmd );

//    std::cout << "Set Pitch to " << semitones << " Ratio: " << pitch_ratio_str << '\n';
}


void Player::setVolume( double vol )
{
    volume = vol;
    mpv_set_property(mpv, "volume", MPV_FORMAT_DOUBLE, &volume);
//    std::cout << "Mpv set property succeeded\n";
}


// Jump to timestamp
void Player::seek(double seek_pos) {
    std::string seek_pos_str{ std::to_string(seek_pos) };
    const char *seek_cmd[] = {"seek", seek_pos_str.c_str(), "absolute", NULL};
    mpv_command(mpv, seek_cmd);
}

// Restart video
void Player::restart() {
    seek(0.0);
    play();
}

