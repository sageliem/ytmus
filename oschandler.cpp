// OSC handler

#include "oschandler.hpp"
#include "controller.hpp"

#include <iostream>
#include <unistd.h>

OscHandler::OscHandler(int thread_init)
    : thread{thread_init}, received{0}, controller{nullptr} {
  if (!thread.is_valid()) {
    std::cout << "Bad thread.\n";
    return;
  }

  thread.set_callbacks(
      [this]() { std::cout << "Thread init: " << &thread << '\n'; },
      []() { std::cout << "Thread cleanup\n"; });

  std::cout << "URL: " << thread.url() << '\n';
}

void OscHandler::init(Controller *controller) {
  this->controller = controller;

  setHandlerMethods();

  thread.start();
}

void OscHandler::setHandlerMethods() {

  // Add handler methods
  thread.add_method("/test", "i", [this](lo_arg **argv, int) {
    std::cout << "/test (" << (++received)
              << "): "
              // To access argv: index it, then access correct data type
              << argv[0]->i << '\n';

    return 0;
  });

  // Handler for notes (buffer, seekpos, duration, pitch, speed)
  // TODO add fine pitch
  thread.add_method("/note", "ifiif", [this](lo_arg **argv, int) {
    int player = argv[0]->i;
    double seekPos = argv[1]->f;
    int duration = argv[2]->i; // milliseconds
    int pitch = argv[3]->i;
    double speed = argv[4]->f;

    controller->ctlPitch(player, pitch);
    controller->ctlSpeed(player, speed);
    controller->ctlSeek(player, seekPos);
    controller->ctlVolume(player, 1.0);
    controller->ctlVolume(player, 0.0, duration);
    return 0;
  });

  // Handler for seek messages (buffer, seekPos)
  thread.add_method("/seek", "if", [this](lo_arg **argv, int) {
    // std::cout << "Received a seek message\n";
    int player = argv[0]->i;
    double seekPos = argv[1]->f;
    this->controller->ctlSeek(player, seekPos, 0);
    //
    // std::cout << "Argv[0] is " << argv[0] -> i32 << '\n';
    // std::cout << "Argv[1] is " << argv[1] -> f32 << '\n';
    // this->controller->ctlSeek( static_cast<double>(argv[1] -> f32) );

    return 0;
  });

  // Loop (buffer, loopStart, loopLength)
  thread.add_method("/loop", "iff", [this](lo_arg **argv, int) {
    // std::cout << "Received a seek message\n";
    int player = argv[0]->i;
    double loopStart = argv[1]->f;
    double loopLength = argv[2]->f;
    this->controller->ctlLoopStart(player, loopStart);
    // this->controller->ctlLoopEnd(player, loopStart + loopLength);
    this->controller->ctlLoopLength(player, loopLength);

    // std::cout << "Argv[0] is " << argv[0] -> i32 << '\n';
    // std::cout << "Argv[1] is " << argv[1] -> f32 << '\n';
    // this->controller->ctlSeek( static_cast<double>(argv[1] -> f32) );

    return 0;
  });

  // Handler for volume messages
  thread.add_method("/vol", "if", [this](lo_arg **argv, int) {
    int player = argv[0]->i;
    double vol = argv[1]->f;

    this->controller->ctlVolume(player, vol);

    return 0;
  });

  /*
// Send some test messages
lo::Address a( "localhost", "5050" );

a.send( "/test", "i", 7890987 );
// seek test
a.send( "/seek", "if", 1, 3.1415f );
*/

  /*int tries = 2000;
  while (received < 20 && --tries > 0)
  {
      usleep(30*1000);
  }

  if (tries <= 0)
  {
      std::cout << "Waited too long for messages.\n";
      return;
  }

  std::cout << "Success!\n";
  */
}
