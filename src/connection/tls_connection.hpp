#pragma once

#include "connection.hpp"

#include <iostream>

class TlsConnection : public Connection<beast::ssl_stream<beast::tcp_stream>> {
  public:
    TlsConnection(net::io_context &ioc, const std::string &host,
                  const std::string &port, const std::string &ca_file,
                  const std::string &cert_file, const std::string &key_file)
        : ctx_([&] {
              ssl::context ctx{ssl::context::tls_client};
              ctx.set_default_verify_paths();
              ctx.load_verify_file(ca_file);
              ctx.use_certificate_chain_file(cert_file);
              ctx.use_private_key_file(key_file, ssl::context::pem);
              ctx.set_verify_mode(ssl::verify_peer);
              return ctx;
          }()),
          stream_(ioc, ctx_), Connection(&stream_) {

        tcp::resolver resolver(ioc);
        auto endpoints = resolver.resolve(host, port);
        beast::get_lowest_layer(stream_).connect(endpoints);
        stream_.handshake(ssl::stream_base::client);
    }

    ~TlsConnection() override {
        boost::system::error_code ec;
        std::cerr << "Shutting down TLS connection" << std::endl;
        stream_.shutdown(ec);
        std::cerr << "Done" << std::endl;
    }

  private:
    ssl::context ctx_;
    Connection::stream_type stream_;
};