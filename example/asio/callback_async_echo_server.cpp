//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2025 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/asio.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>

using boost::asio::ip::tcp;

// std::enable_shared_from_this는 객체가 자신의 shared_ptr를 생성할 수 있도록
// 하는 클래스 템플릿. 객체 안에서 shared_from_this()를 호출해서 자기 자신의
// shared_ptr를 얻을 수 있음. shared_from_this 호출 시 해당 함수를 호출한
// instance가 저장되어 있는 shared_ptr에서 reference count가 1 증가됨. 이렇게
// 얻은 self의 shared_ptr은 callback에 전달되어 callback이 종료될 때 까지 객체가
// destroy되지 않도록 함.
class session : public std::enable_shared_from_this<session> {
  public:
    session(tcp::socket socket) : socket_(std::move(socket)) {}

    void start() {
        // do_read로 시작.
        do_read();
    }

  private:
    /*
     * Session 객체가 async하게 수행되는 read/write loop을 구현하는 방식에 주목.
     * 매우 직관적이지 않은 방식이죠...
     */
    void do_read() {
        auto self(shared_from_this());
        // 2) shared_from_this() 호출 시 session 객체가 저장되어 있는
        // shared_ptr에서 reference count가 1 증가되어 2가 됨.
        //
        // Client에서 request를 보내오면 실행될 callback을 등록.
        socket_.async_read_some(
            boost::asio::buffer(data_, max_length),
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    do_write(length);
                    // 4) do_write에서 async_write를 호출하면서 callback에
                    // shared_from_this()로 얻은 self를 전달. 즉 write
                    // callback에서 reference count 하나를 증가시킴.
                }
                // 5) 이 callback이 종료되어도, write callback이 종료되기까지는
                // session 객체가 destroy되지 않음.
            });
    }

    void do_write(std::size_t length) {
        auto self(shared_from_this());
        boost::asio::async_write(
            socket_, boost::asio::buffer(data_, length),
            [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    do_read();
                }
                // read/write callback에 error code가 전달된 경우, 서로를
                // 호출하는 일 없이 종료되므로 이 때 reference count가 0이 되어
                // session 객체가 최종 destroy됨.
            });
    }

    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};

class server {
  public:
    server(boost::asio::io_context &io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        // 객체 생성 시 accepter_를 초기화하고, do_accept()를 호출하여
        // client에서 하나의 접속이 들어오면 async_accept에 등록된 callback이
        // 실행되도록 함.
        do_accept();
    }

  private:
    void do_accept() {
        acceptor_.async_accept(
            // Client에서 첫 접속이 들어오면 accepter는 새로운 tcp::socket
            // 객체를 생성하여 callback에 전달함. 이 때 발생한 error code도
            // callback에 전달 (callback 안에서 exception이 발생하도록 할 방법이
            // 없으므로).
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    // session 객체를 생성하되, 해당 reference를 저장할 곳이
                    // 없으므로 shared_ptr로 생성하여 session 객체가 callback이
                    // 종료되어도 destroy되지 않도록 함.
                    std::make_shared<session>(std::move(socket))->start();
                    // socket을 rvalue로 만들어서 전달, session이 socket의
                    // 소유권을 가지도록 함. session의 start를 호출.
                    //
                    // 1) session 객체 생성 시 shared_ptr의 reference count는
                    // 1이 됨.
                }

                // 다음 client 접속을 기다리도록 새로운 lambda 객체를
                // async_accept에 등록함.
                do_accept();

                // 3) 이 callback이 종료될 때 self의 reference count가 2에서 1
                // 감소되어 1이 됨. 사실, async_read의 callback이 먼저 호출되고
                // 종료된다면 거기서 먼저 reference가 감소되지만, 중요한건
                // async_read의 callback이 종료된 상황이 아니라면 async_accept에
                // 등록된 callback이 종료되는 것으로 인해 session 객체가
                // destroy되지 않는다는 것을 이해해야 함.
            });
    }

    tcp::acceptor acceptor_;
};

int main(int argc, char *argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: async_tcp_echo_server <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;

        server s(io_context, std::atoi(argv[1]));

        io_context.run();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}