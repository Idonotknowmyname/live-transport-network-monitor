#ifndef NET_MON_LOGGING
#define NET_MON_LOGGING

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <boost/beast.hpp>

#include <string>
#include <iostream>
#include <iomanip>

#define LOG_LEVEL_WIDTH 14

namespace mm {


    void LogErr(boost::system::error_code ec)
    {
        std::cerr << "[" << std::setw(14) << std::this_thread::get_id() << "] "
                << (ec ? "Error: " : "No error: OK")
                << (ec ? ec.message() : "")
                << std::endl;
    }

    template<typename StreamType, typename T, typename... Types>
    void Log(StreamType& stream, T firstArg, Types... args)
    {
        stream << firstArg << " ";
        if constexpr(sizeof...(args) > 0) {
            Log(stream, args...);
        } else {
            stream << std::endl;
        }
    }

    template<typename... Types>
    void LogInfo(Types... args) {
        Log(std::cout, args...);
    }
}

#endif