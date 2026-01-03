//
// blocking_tcp_echo_client.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2025 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/asio.hpp>
#include <cstdlib>
#include <cstring>
#include <iostream>

using boost::asio::ip::tcp;

enum { max_length = 1024 };

int main(int argc, char *argv[]) {
    try {
        if (argc != 3) {
            std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;

        tcp::socket s(io_context);
        tcp::resolver resolver(io_context);
        boost::asio::connect(s, resolver.resolve(argv[1], argv[2]));

        boost::asio::streambuf sbuf;
        char reply[max_length];
        {
            constexpr const char *request = "hello\r\nhello\r\n";
            size_t request_length = std::strlen(request);
            boost::asio::write(s, boost::asio::buffer(request, request_length));
        }
        {
            std::cout << "Waiting for reply...\n";
            size_t len = boost::asio::read_until(s, sbuf, "\r\n");
            auto read = sbuf.data();
            std::cout.write(static_cast<const char *>(read.data()),
                            read.size());
            sbuf.consume(len);
        }
        // {
        //     constexpr const char *request = "hello";
        //     size_t request_length = std::strlen(request);
        //     boost::asio::write(s, boost::asio::buffer(request, request_length));
        // }
        // {
        //     constexpr const char *request = "\n";
        //     size_t request_length = std::strlen(request);
        //     boost::asio::write(s, boost::asio::buffer(request, request_length));
        // }
        // {
        //     size_t len = boost::asio::read_until(s, sbuf, "\n");
        //     auto read = sbuf.data();
        //     std::cout.write(static_cast<const char *>(read.data()),
        //                     read.size());
        //     sbuf.consume(len);
        // }
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}