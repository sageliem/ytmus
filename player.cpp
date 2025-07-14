// Player.cpp
// Sage Liem 2025
// mpv wrapper class providing play, pause, loop, and seek functionality

#include "player.hpp"
#include <cmath>
#include <cstdio>
#include <iostream>
#include <mutex>

// Constructor for Player object
Player::Player()
    //  : port{},
    : path{}, title{}, loop_start{}, loop_end{}, loop_length{}, duration{},
      rate{1.0}, pitch{0.0}, volume{50.0} {
  player_mutex.lock();
  mpv = mpv_create();
  std::cout << "Created mpv instance\n";

  mpv_initialize(mpv);
  std::cout << "Initialized mpv instance\n";

  // Defaults (no video)
  //    mpv_set_option_string(mpv, "vo", "null");

  // Video on
  mpv_set_option_string(mpv, "vo", "gpu");

  // mpv_set_option_string(mpv, "gpu-api", "vulkan");
  // mpv_set_option_string(mpv, "hwdec", "auto-safe");

  // May have to replace with a command if using more filters
  // Needs to be named filter @rb to access later
  mpv_set_option_string(mpv, "af", "@rb:rubberband=pitch-scale=1.0");
  player_mutex.unlock();
}

// Destructor terminates mpv and destroys mpv client object
Player::~Player() {
  player_mutex.lock();
  mpv_terminate_destroy(mpv);
  player_mutex.unlock();
  std::cout << "Destroyed mpv instance\n";
}

// Load a video
void Player::load(const std::string &url) {
  path = url;
  const char *load_cmd[] = {"loadfile", url.c_str(), "replace", NULL};
  std::cout << "Attempting to load file at " << url << '\n';

  player_mutex.lock();
  mpv_command(mpv, load_cmd);
  player_mutex.unlock();

  // Wait for load successful
  bool loaded = false;
  while (!loaded) {
    player_mutex.lock();
    mpv_event *event =
        mpv_wait_event(mpv, 0); // wait for an event (non-blocking)
    player_mutex.unlock();
    if (event->event_id == MPV_EVENT_FILE_LOADED)
      loaded = true;
  }
  std::cout << "File loaded: " << path << std::endl;

  player_mutex.lock();
  // Get video title
  mpv_get_property(mpv, "media-title", MPV_FORMAT_STRING, &title);

  // Set duration
  mpv_get_property(mpv, "duration", MPV_FORMAT_DOUBLE, &duration);
  player_mutex.unlock();

  loop_end = duration;
}

void Player::update() {
  mpv_event *event = mpv_wait_event(mpv, 0.0); // non-blocking
  if (event->event_id == MPV_EVENT_END_FILE) {
    restart();
  } else {
    double currentPos = getCurrentPos();
    if ((currentPos > loop_end) || (currentPos < loop_start - 0.1)) {
      seek(loop_start);
    }
  }
}

// Private helpers
// Check if timestamp is within video length
bool Player::isValidTimestamp(double timestamp) {
  return ((timestamp >= 0) && (timestamp <= duration));
}

// Getters
bool Player::isPlaying() {
  int paused{0};
  mpv_get_property(mpv, "paused", MPV_FORMAT_FLAG, &paused);
  return !paused;
}

double Player::getCurrentPos() {
  double currentPos{};
  player_mutex.lock();
  mpv_get_property(mpv, "time-pos", MPV_FORMAT_DOUBLE, &currentPos);
  player_mutex.unlock();
  return currentPos;
}

double Player::getDuration() { return duration; }

double Player::getLoopStart() { return loop_start; }

double Player::getLoopEnd() { return loop_end; }

double Player::getLoopLength() { return loop_length; }

double Player::getSpeed() { return rate; }

double Player::getPitch() { return pitch; }

double Player::getVolume() { return volume; }

std::string_view Player::getTitle() {
  std::string_view title_str(title);
  return title_str;
}

std::string_view Player::getPath() { return path; }

double Player::getProgress() { return getCurrentPos() / getDuration(); }

// Playback controls
// Play
void Player::play() {
  int paused = 0; // TODO check if this is necessary, could be null if not
  player_mutex.lock();
  mpv_set_property(mpv, "pause", MPV_FORMAT_FLAG, &paused);
  player_mutex.unlock();
}

// Pause
void Player::pause() {
  int paused = 1;
  player_mutex.lock();
  mpv_set_property(mpv, "play", MPV_FORMAT_FLAG, &paused);
  player_mutex.unlock();
}

// Set loop start and end
void Player::set_loop(double start, double end) {
  if (!isValidTimestamp(start) || !isValidTimestamp(end))
    return;

  loop_start = start;
  if (end > start + 0.1) {
    loop_end = end;
  } else {
    loop_end = loop_start + 0.1;
  }
}

// Set loop start
void Player::set_loop_start(double start) {
  if (!isValidTimestamp(start))
    return;

  if (start < loop_end - 0.01) {
    loop_start = start;
  } else {
    loop_start = loop_end - 0.01;
  }
  set_loop_length(loop_length);
}

// Set loop end
void Player::set_loop_end(double end) {
  if (!isValidTimestamp(end))
    return;

  if (end > loop_start + 0.01) {
    loop_end = end;
  } else {
    loop_end = loop_start + 0.01;
  }
}

void Player::set_loop_length(double length) {
  if (!isValidTimestamp(loop_start + length))
    return;
  loop_length = length;
  set_loop_end(loop_start + length);
}

void Player::set_rate(double new_rate) {
  rate = new_rate;
  mpv_set_property(mpv, "speed", MPV_FORMAT_DOUBLE, &new_rate);
}

void Player::setPitch(int semitones) {
  double semitone_ratio = 1.0595;
  double pitch_ratio = pow(semitone_ratio, static_cast<double>(semitones));
  pitch = semitones;

  char pitch_ratio_str[64];
  snprintf(pitch_ratio_str, sizeof(pitch_ratio_str), "%.8f", pitch_ratio);

  const char *pitch_cmd[] = {"af-command", "rb", "set-pitch", pitch_ratio_str,
                             NULL};

  player_mutex.lock();
  mpv_command(mpv, pitch_cmd);
  player_mutex.unlock();
}

void Player::setFinePitch(double newFinePitch) {
  // TODO
}

void Player::setVolume(double vol) {
  if (vol <= 0.0 || vol >= 100.0)
    return;
  volume = vol;

  player_mutex.lock();
  mpv_set_property(mpv, "volume", MPV_FORMAT_DOUBLE, &volume);

  player_mutex.unlock();
}

// Jump to timestamp
void Player::seek(double seek_pos) {
  if (!isValidTimestamp(seek_pos))
    return;

  std::string seek_pos_str{std::to_string(seek_pos)};
  const char *seek_cmd[] = {"seek", seek_pos_str.c_str(), "absolute", NULL};

  player_mutex.lock();
  mpv_command(mpv, seek_cmd);
  player_mutex.unlock();
}

// Restart video
void Player::restart() {
  seek(0.0);
  play();
}

double Player::normalizeTimestamp(double timestamp) {
  return timestamp / duration;
}
