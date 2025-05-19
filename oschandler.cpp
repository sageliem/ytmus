// OSC handler

#include "oschandler.hpp"
#include <iostream>
#include <unistd.h>

OscHandler::OscHandler( int thread_init )
  : thread { thread_init },
    received { 0 }
{
    if ( !thread.is_valid() )
    {
        std::cout << "Bad thread.\n";
        return;
    }

    thread.set_callbacks( 
        [this]() { std::cout << "Thread init: " << &thread << '\n'; },
        []() { std::cout << "Thread cleanup\n"; } );

    std::cout << "URL: " << thread.url() << '\n';

    }

void OscHandler::init( Controller* controller )
{

    // Add handler methods
    thread.add_method( "/test", "i",
        [this](lo_arg **argv, int)
        {
            std::cout << "/test (" << (++received) << "): "
            << argv[0] -> i << '\n';
        }
    );

    thread.start();

    // Send some test messages
    lo::Address a( "localhost", "9000" );

    a.send( "/test", "i", 7890987 );


    int tries = 200;
    while (received < 1 && --tries > 0)
    {
        usleep(20*1000);
    }

    if (tries <= 0)
    {
        std::cout << "Waited too long for messages.\n";
        return;
    }

    std::cout << "Success!\n";
}
