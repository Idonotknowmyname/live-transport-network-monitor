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
    // Initialize constants
    const std::string host = "ltnm.learncppthroughprojects.com";
    const std::string port = "80";
    const std::string path = "/echo";
    const std::string message = "Hello World Wide Web!";

    // Always start with an I/O context object.
    boost::asio::io_context ioc {};

    // Initialize error container
    boost::system::error_code ec {};

    // Initialize the TCP resolver
    tcp::resolver resolver {ioc};

    // Create a websocket stream object
    boost::beast::websocket::stream<boost::beast::tcp_stream> ws {ioc};
    
    // Initialize buffer to store the response from the API
    boost::beast::flat_buffer resp;

    // Define the chain of callbacks in reversed order (painful)
    auto onRead = [&resp](boost::system::error_code ec, std::size_t transferred) {
        if (ec) {
            Log("Failed async_read");
            LogErr(ec);
            return;
        }

        std::string response = boost::beast::buffers_to_string(resp.data());

        Log("Received message");
        Log(response);

    };

    auto onWrite = [&ws, &resp, &onRead](boost::system::error_code ec, std::size_t transferred) {
        if (ec) {
            Log("Failed async_write");
            LogErr(ec);
            return;
        }

        ws.async_read(resp, onRead);
    };

    auto onHandshake = [&ws, &message, &onWrite](boost::system::error_code ec) {
        if (ec) {
            Log("Failed async_handshake");
            LogErr(ec);
            return;
        }

        boost::asio::const_buffer wbuffer {message.c_str(), message.size()};
        ws.async_write(wbuffer, onWrite);
    };

    auto onConnect = [&ws, &host, &path, &onHandshake](boost::system::error_code ec) {
        if (ec) {
            Log("Failed async_connect");
            LogErr(ec);
            return;
        }

        ws.async_handshake(host, path, onHandshake);
    };

    auto onResolve = [&ws, &onConnect](boost::system::error_code ec, tcp::resolver::results_type resResults) {
        if (ec) {
            Log("Failed async_resolve");
            LogErr(ec);
            return;
        }

        ws.next_layer().async_connect(*resResults, onConnect);
    };

    // Start the chain of resolvers with the first async method
    resolver.async_resolve(host, port, onResolve);

    // Start the loop in the io context
    ioc.run();

    return 0;
}