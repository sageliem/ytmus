// OSC handler

#include "oschandler.hpp"
#include "controller.hpp"

#include <iostream>
#include <unistd.h>

OscHandler::OscHandler( int thread_init )
  : thread { thread_init },
    received { 0 },
    controller { nullptr }
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
    this->controller = controller;

    // Add handler methods
    thread.add_method( "/test", "i",
        [this](lo_arg **argv, int)
        {
            std::cout << "/test (" << (++received) << "): "
            // To access argv: index it, then access correct data type
            << argv[0] -> i << '\n';

            return 0;
        }
    );

    // Handler for seek messages
    thread.add_method( "/seek", "if",
            [this](lo_arg **argv, int)
            {
                std::cout << "Received a seek message\n";
                this->controller->ctlBufSelect( argv[0] -> i );
                std::cout << "Argv[0] is " << argv[0] -> i32 << '\n';
                std::cout << "Argv[1] is " << argv[1] -> f32 << '\n';
                this->controller->ctlSeek( static_cast<double>(argv[1] -> f32) );


                return 0;
            });

    // Handler for mute messages
    thread.add_method( "/seek", "if",
            [this](lo_arg **argv, int)
            {
                this->controller->ctlBufSelect( argv[0] -> i );
                this->controller->ctlSeek( argv[1] -> f );

                std::cout << "Argv[0] is " << argv[0] -> i << '\n';
                std::cout << "Argv[1] is " << argv[1] -> f << '\n';

                return 0;
            });

    // Handler for seek messages
    thread.add_method( "/seek", "if",
            [this](lo_arg **argv, int)
            {
                this->controller->ctlBufSelect( argv[0] -> i );
                this->controller->ctlSeek( argv[1] -> f );

                std::cout << "Argv[0] is " << argv[0] -> i << '\n';
                std::cout << "Argv[1] is " << argv[1] -> f << '\n';

                return 0;
            });




    thread.start();

    /*
    // Send some test messages
    lo::Address a( "localhost", "5050" );

    a.send( "/test", "i", 7890987 );
    // seek test
    a.send( "/seek", "if", 1, 3.1415f );
    */


    int tries = 2000;
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
}
