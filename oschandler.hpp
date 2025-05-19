// OSC Handler
//
//

#include <array>
#include <iostream>
#include <atomic>

#include <lo/lo.h>
#include <lo/lo_cpp.h>

class Controller;

class OscHandler
{
    lo::ServerThread thread;
    std::atomic<int> received;
    Controller* controller;
public:
    OscHandler(int thread_init);
    void init(Controller* controller);
};
