// OSC Handler
//
//

#include <array>
#include <atomic>
#include <iostream>

#include <lo/lo.h>
#include <lo/lo_cpp.h>

class Controller;

class OscHandler {
  lo::ServerThread thread;
  std::atomic<int> received;
  Controller *controller;

  void setHandlerMethods();

public:
  OscHandler(int thread_init);
  void init(Controller *controller);
};
