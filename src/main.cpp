#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <boost/beast.hpp>

#include <iomanip>
#include <iostream>
#include <thread>

using tcp = boost::asio::ip::tcp;

void LogErr(boost::system::error_code ec)
{
    std::cerr << "[" << std::setw(14) << std::this_thread::get_id() << "] "
              << (ec ? "Error: " : "No error: OK")
              << (ec ? ec.message() : "")
              << std::endl;
}

void Log(std::string message)
{
    std::cout << message << std::endl;
}

int main()
{
    std::string host = "ltnm.learncppthroughprojects.com";
    std::string port = "80";
    std::string path = "/echo";
    std::string message = "Hello World Wide Web!";

    // Always start with an I/O context object.
    boost::asio::io_context ioc {};

    // Under the hood, socket.connect uses I/O context to talk to the socket
    // and get a response back. The response is saved in ec.
    boost::system::error_code ec {};
    tcp::resolver resolver {ioc};

    auto resolverIt {resolver.resolve(host, port, ec)};

    if (ec) {
        LogErr(ec);
        return -1;
    }

    // Create a TCP socket
    tcp::socket socket {ioc};
    // Connect the socket to the IP address of the server
    socket.connect(*resolverIt, ec);

    // If failed to connect, error out
    if (ec) {
        LogErr(ec);
        return -1;
    }

    // Create a websocket to use the TCP stream
    boost::beast::websocket::stream<boost::beast::tcp_stream> ws {std::move(socket)};
    // Perform handshake
    ws.handshake(host, path, ec);

    if (ec) {
        LogErr(ec);
        return -1;
    }

    // Write message
    boost::asio::const_buffer wbuffer {message.c_str(), message.size()};
    ws.write(wbuffer, ec);

    if (ec) {
        LogErr(ec);
        return -1;
    }

    // Read message
    boost::beast::flat_buffer resp;
    ws.read(resp);
    std::string response = boost::beast::buffers_to_string(resp.data());

    // Relay the message
    Log("Message received:");
    Log(response);

    return 0;
}