#pragma once

#include "connection.hpp"

class SocketConnection : public Connection<beast::tcp_stream> {
  public:
    SocketConnection(net::io_context &ioc, const std::string &host,
                     const std::string &port)
        : stream_(ioc), Connection(&stream_) {
        tcp::resolver resolver(ioc);
        auto endpoints = resolver.resolve(host, port);
        stream_.connect(endpoints);
    }

    ~SocketConnection() override {
        boost::system::error_code ec;
        stream_.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both,
                                  ec);
        stream_.socket().close(ec);
    }

  private:
    Connection::stream_type stream_;
};