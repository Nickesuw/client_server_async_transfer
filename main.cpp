#include <iostream>
#include <string>

#include <boost/asio/io_service.hpp>
#include <boost/thread.hpp>

#include "client.h"


int main()
{
while(true)
{
    // Вот это выглядит очень некрасиво, но если параметры будут поступать не из терминала, то будет в разы проще. Преобразование в char * нужно для вызова resolver

    std::string address1;
    std::string  port1;
    std::string  filePath2;

    std::cout<<"Enter address:";
    getline(std::cin,address1);
    std::cout<<"Enter a port:";
    getline(std::cin,port1);
    std::cout<<"Enter filepath:";
    getline(std::cin,filePath2);

    const char* filePath=filePath2.data();
    const char* address=address1.data();
    const char* port=port1.data();



    try
    {

        boost::asio::io_service ioService;

        boost::asio::ip::tcp::resolver resolver(ioService);

        auto endpointIterator = resolver.resolve({ address, port });

        Client client(ioService, endpointIterator, filePath);

        boost::thread* thr = new boost::thread(boost::bind(&Client::thread_for_commands,&client));

        thr->join();

        ioService.run();
    }

    catch (std::fstream::failure& e)
    {
        std::cerr << e.what() << "\n";

    }

    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}
    return 0;
}
