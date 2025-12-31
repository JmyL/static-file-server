//
// blocking_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2025 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/asio.hpp>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <utility>

using boost::asio::ip::tcp;

const int max_length = 1024;

void session(tcp::socket sock) {
    try {
        auto sbuf = boost::asio::streambuf(max_length);
        for (;;) {
            auto len = boost::asio::read_until(sock, sbuf, "\n");
            sbuf.consume(len);

            boost::asio::write(sock, boost::asio::buffer("world\n", 6));
        }
    } catch (boost::system::system_error &e) {
        if (e.code() == boost::asio::error::eof) {
            std::cout << "Connection closed by peer.\n";
        } else {
            std::cerr << "Boost System Error in thread: " << e.what() << "\n";
        }
    } catch (std::exception &e) {
        std::cerr << "Exception in thread: " << e.what() << "\n";
    }
}

void server(boost::asio::io_context &io_context, unsigned short port) {
    tcp::acceptor a(io_context, tcp::endpoint(tcp::v4(), port));
    for (;;) {
        std::thread(session, a.accept()).detach();
    }
}

int main(int argc, char *argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: blocking_tcp_echo_server <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;

        server(io_context, std::atoi(argv[1]));
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}