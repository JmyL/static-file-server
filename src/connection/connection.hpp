#pragma once

#include <boost/asio.hpp>

template <typename Socket> class Connection {
    Socket socket_;

  public:
    Connection(Socket &&socket) : socket_(std::move(socket)) {}
    auto read_line(char *data, size_t length) -> size_t {
        return socket_.read_some(boost::asio::buffer(data, length));
    }
    void write_line(char *data, size_t length) {
        boost::asio::write(socket_, boost::asio::buffer(data, length));
    }
};