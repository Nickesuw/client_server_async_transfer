#include <iostream>

#include <boost/asio/io_service.hpp>
#include <boost/thread.hpp>

#include "server.h"


int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: <port> <workDirectory>\n";
        return 1;
    }

    try
    {
        boost::asio::io_service ioService;

        Server server(ioService, std::stoi(argv[1]), argv[2]);

        boost::thread* thr = new boost::thread(boost::bind(&Server::thread_for_commands,&server));

        thr->join();

        ioService.run();
    }
    catch (std::exception& e)

    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
