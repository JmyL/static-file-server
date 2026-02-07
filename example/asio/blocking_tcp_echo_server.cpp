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

// https://www.boost.org/doc/libs/latest/doc/html/boost_asio/example/cpp11/echo/blocking_tcp_echo_server.cpp
// NOTE: 위 경로의 예제에서 session 함수의 에러 처리 방식만 exception을 사용하는
// 것으로 변경됨.
void session(tcp::socket sock) {
    try {
        // Client와의 연결이 유지되는 동안 session은 계속해서 client의 request를
        // 기다리고, request가 들어오면 이를 처리함.
        for (;;) {
            char data[max_length];
            size_t length = sock.read_some(boost::asio::buffer(data));
            boost::asio::write(sock, boost::asio::buffer(data, length));
            // 읽은 내용을 그대로 client에게 다시 보내는 echo 동작.
        }
    } catch (boost::system::system_error &e) {
        if (e.code() == boost::asio::error::eof) {
            std::cout << "Connection closed by peer.\n";
            return;
        } else {
            std::cerr << "Boost System Error in thread: " << e.what() << "\n";
            return;
        }
    } catch (std::exception &e) {
        std::cerr << "Exception in thread: " << e.what() << "\n";
    }
}

void server(boost::asio::io_context &io_context, unsigned short port) {
    tcp::acceptor a(io_context, tcp::endpoint(tcp::v4(), port));
    for (;;) {
        // a.accept()는 새로운 클라이언트가 접속할 때 까지 기다림.
        // 접속 시 새로운 tcp::socket 객체를 생성하고 이를 반환 (rvalue).
        std::thread(session, a.accept()).detach();
        // 각 client와의 연결을 server 입장에서는 session이라고 부름.
        // 각 session이 개별 thread에서 처리됨.
        // 이는 1000개 client 동시 접속 시 1000개 thread가 생성되는 것으로,
        // hyperthread 지원 CPU 500개가 있지 않은 이상 성능 저하가 발생함
        // (context switching overhead).
    }
}

int main(int argc, char *argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: blocking_tcp_echo_server <port>\n";
            return 1;
        }

        // boost asio에서 io 작업을 처리하는 핵심 객체.
        boost::asio::io_context io_context;

        server(io_context, std::atoi(argv[1]));
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}