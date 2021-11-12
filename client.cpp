#include <string>
#include <iostream>

#include <boost/filesystem/path.hpp>
#include <boost/log/trivial.hpp>
#include <boost/thread/thread.hpp>

#include "client.h"

Client::Client(IoService& t_ioService, TcpResolverIterator t_endpointIterator,
               std::string const& t_path)
    : m_ioService(t_ioService), m_socket(t_ioService),
      m_endpointIterator(t_endpointIterator), m_path(t_path)
{
    doConnect();
    openFile(m_path);
}

void Client::openFile(std::string const& t_path)
{
    try
    {
        m_sourceFile.open(t_path, std::ios_base::binary | std::ios_base::ate);
        if (m_sourceFile.fail())
            throw std::fstream::failure("Failed while opening file " + t_path);

        m_sourceFile.seekg(0, m_sourceFile.end);
        auto fileSize = m_sourceFile.tellg();
        m_sourceFile.seekg(0, m_sourceFile.beg);

        std::ostream requestStream(&m_request);
        boost::filesystem::path p(t_path);
        requestStream << p.filename().string() << "\n" << fileSize << "\n\n";
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

}

void Client::doConnect()
{
    try
    {
        boost::asio::async_connect(m_socket, m_endpointIterator,
                                   [this](boost::system::error_code ec, TcpResolverIterator)
        {
            if (!ec)
            {
                writeBuffer(m_request);
            }
            else

            {
                std::cout << "Coudn't connect to host. Please run server "
                             "or check network connection.\n";
                std::cerr << "Error: " << ec.message();
            }
        });

    }

    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

}

void Client::thread_for_commands()
{
    boost::asio::io_service io_service;

    //socket creation
        boost::asio::ip::tcp::socket socket(io_service);

    //connection
         socket.connect( boost::asio::ip::tcp::endpoint( boost::asio::ip::address::from_string("127.0.0.1"), 1234 ));

    // request/message from client
         const std::string msg = "Hello from Client!\n";
         boost::system::error_code error;
         boost::asio::write( socket, boost::asio::buffer(msg), error );
         if( !error ) {
              std::cout << "Client sent hello message!" << std::endl;
         }
         else {
              std::cout << "send failed: " << error.message() << std::endl;
         }

    // getting a response from the server
         boost::asio::streambuf receive_buffer;
         boost::asio::read(socket, receive_buffer, boost::asio::transfer_all(), error);
         if( error && error != boost::asio::error::eof ) {
              std::cout << "receive failed: " << error.message() << std::endl;
         }
         else {
              const char* data = boost::asio::buffer_cast<const char*>(receive_buffer.data());
              std::cout << data << std::endl;
         }
}

void Client::doWriteFile(const boost::system::error_code& t_ec)
{
    if (!t_ec) {
        if (m_sourceFile) {
            m_sourceFile.read(m_buf.data(), m_buf.size());
            if (m_sourceFile.fail() && !m_sourceFile.eof())
            {
                auto msg = "Failed while reading file";
                std::cerr << msg;
                throw std::fstream::failure(msg);
            }
            std::stringstream ss; //
            ss << "Send " << m_sourceFile.gcount() << " bytes, total: "
               << m_sourceFile.tellg() << " bytes";
            std::cout << ss.str();
            std::cout << ss.str() << std::endl;

            auto buf = boost::asio::buffer(m_buf.data(), static_cast<size_t>(m_sourceFile.gcount()));

            writeBuffer(buf);
        }
    }
    else
    {
        std::cerr << "Error: " << t_ec.message();
    }
}
