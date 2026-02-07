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
#include <string_view>
#include <thread>
#include <utility>

namespace net = boost::asio;
using net::ip::tcp;
using namespace std::string_view_literals;

const int max_length = 1024;

void session(tcp::socket sock) {
    try {
        auto sbuf = net::streambuf(max_length);
        for (;;) {
            auto len = net::read_until(sock, sbuf, "\r\n\r\n");
            std::cout << "Received " << len << " bytes\n";
            sbuf.consume(len);

            constexpr auto response =
                "HTTP/1.1 200 OK\r\n"
                "Server: nginx/1.29.4\r\n"
                "Date: Sat, 03 Jan 2026 15:22:53 GMT\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 0\r\n"
                "Last-Modified: Sun, 28 Dec 2025 17:25:28 GMT\r\n"
                "Connection: keep-alive\r\n"
                "ETag: \"69516808-4a8\"\r\n"
                "Accept-Ranges: bytes\r\n"
                "\r\n"sv;

            auto write_len =
                net::write(sock, net::buffer(response.data(), response.size()));
            std::cout << "Sent " << write_len << " bytes\n";
        }
    } catch (boost::system::system_error &e) {
        if (e.code() == net::error::eof) {
            std::cout << "Connection closed by peer.\n";
        } else {
            std::cerr << "Boost System Error in thread: " << e.what() << "\n";
        }
    } catch (std::exception &e) {
        std::cerr << "Exception in thread: " << e.what() << "\n";
    }
}

void server(net::io_context &io_context, unsigned short port) {
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

        net::io_context io_context;

        server(io_context, std::atoi(argv[1]));
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}