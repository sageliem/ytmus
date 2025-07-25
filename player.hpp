#pragma once

#include <mutex>
#include <string>
#include <string_view>

extern "C" {
#include <mpv/client.h>
}

// Player struct
class Player {
  mpv_handle *mpv;
  //    jack_port_t* port;
  std::string path;
  char *title;
  double loop_start;
  double loop_end;
  double loop_length;
  double duration;
  double rate;
  double pitch;
  double volume;
  std::mutex player_mutex;
  std::unique_lock<std::mutex> player_lock;

  bool isValidTimestamp(double timestamp);

public:
  Player();
  ~Player();
  void load(const std::string &url);
  //    void init_audio(const std::string &port_name, jack_client_t* client);
  void update();
  void play();
  void pause();
  void set_loop(double start, double end);
  void set_loop_start(double start);
  void set_loop_end(double end);
  void set_loop_length(double length);
  void set_rate(double new_rate);
  void seek(double pos);
  void restart();
  void setPitch(int semitones);
  void setFinePitch(double newFinePitch);
  void setVolume(double vol);
  //    void pitchUpSemitone();
  //    void pitchDownSemitone();

  bool isPlaying();
  double getCurrentPos();
  double getDuration();
  double getLoopStart();
  double getLoopEnd();
  double getLoopLength();
  double getSpeed();
  double getPitch();
  double getVolume();
  double getProgress();
  std::string_view getPath();
  std::string_view getTitle();
  double normalizeTimestamp(double timestamp);

  // Maybe make private
  void fill_audio(float *buffer, int n_frames);
};
