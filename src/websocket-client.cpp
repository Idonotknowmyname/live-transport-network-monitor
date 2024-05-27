#include <network-monitor/websocket-client.h>
#include <network-monitor/logging.h>

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <boost/beast.hpp>

#include <string>
#include <iostream>
#include <iomanip>

using namespace NetworkMonitor;
using tcp = boost::asio::ip::tcp;
using namespace mm;


WebSocketClient::WebSocketClient(
    const std::string& url,
    const std::string& endpoint,
    const std::string& port,
    boost::asio::io_context& ioc
) : url {url},
    endpoint {endpoint},
    port {port},
    resolver {boost::asio::make_strand(ioc)},
    ws {boost::asio::make_strand(ioc)}
    {
}

void WebSocketClient::Connect(
    std::function<void (boost::system::error_code)> onConnect,
    std::function<void (boost::system::error_code,
                        std::string&&)> onMessage,
    std::function<void (boost::system::error_code)> onDisconnect
) {
    onConnect_ = onConnect;
    onMessage_ = onMessage;
    onDisconnect_ = onDisconnect;


    resolver.async_resolve(url, port, [this](auto ec, auto resolverIt){
        if (ec) {
            LogInfo("Failed when trying to resolve host:");
            LogErr(ec);
            return;
        }

        LogInfo("Succeeded in resolving host ", (*resolverIt.begin()).endpoint(), "- attempting to connect...");

        ws.next_layer().async_connect(*resolverIt,
            [this](auto ec) {
                OnConnect(ec);
            }
        );
    });
}

void WebSocketClient::OnConnect(boost::system::error_code ec) {
    if (ec) {
        LogInfo("Failed to connect to host");
        LogErr(ec);
        return;
    }
    
    LogInfo("Succeeded in connecting to host");

    ws.async_handshake(url, endpoint, [this](auto ec) {
        if (ec) {
            LogInfo("Failed to establish websocket handshake");
            LogErr(ec);
            return;
        }
        LogInfo("Succeeded in establishing WS handshake, starting to listen to incoming messages");
        if (onConnect_) {
            onConnect_(ec);
        }

        is_open = true;

        ListenToIncomingMessages();
    });

}

void WebSocketClient::ListenToIncomingMessages()
{
    // Read a message asynchronously. On a successful read, process the message
    // and recursively call this function again to process the next message.
    ws.async_read(rBuffer,
        [this](auto ec, auto nBytes) {
            // Stop processing messages if the connection has been aborted.
            if (ec == boost::asio::error::operation_aborted) {
                // We check the closed_ flag to avoid notifying the user twice.
                LogInfo("Operation aborted error when attempting to read:");
                LogErr(ec);
                if (onDisconnect_ && is_open) {
                    onDisconnect_(ec);
                }
                return;
            } else if (ec)
            {
                if (is_open) {
                    LogInfo("Generic error when attempting to read:");
                    LogErr(ec);
                    return;
                } else {
                    LogInfo("Connection is closed, exiting...");
                    return;
                }
            } else {
                
                LogInfo("Successfully read", nBytes, "bytes");
                
                auto message = boost::beast::buffers_to_string(rBuffer.data());
                LogInfo("Received message:", message);

                if (onMessage_) {
                    onMessage_(ec, std::move(message));
                }
            }
            // Keep listening
            ListenToIncomingMessages();
        }
    );
}

void WebSocketClient::Send(
    const std::string& message,
    std::function<void (boost::system::error_code)> onSend
) {
    LogInfo("Sending message");
    ws.async_write(boost::asio::buffer(message), [onSend](auto ec, auto) {
        LogInfo("Sent message");
        if (onSend) {
            onSend(ec);
        }
    });
}

void WebSocketClient::Close(
    std::function<void (boost::system::error_code)> onClose
) {
    if (is_open) {
        LogInfo("Closing connection");
        is_open = false;
        boost::system::error_code ec;
        ws.next_layer().socket().close(ec);
        if (onClose) {
            onClose(ec);
        }
    }
}

WebSocketClient::~WebSocketClient() {
    LogInfo("Called destructor");
    Close(nullptr);
}
