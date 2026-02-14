#include <boost/asio.hpp>
#include <boost/asio/stream_file.hpp>
#include <cstdlib>
#include <format>
#include <iostream>
#include <memory>
#include <string_view>
#include <utility>

namespace net = boost::asio;
using net::ip::tcp;
using namespace std::string_view_literals;

const int max_length = 1024;

struct ReadmeContent {

    ReadmeContent(net::io_context &io_context) {
        auto file = net::stream_file(io_context, "README.md",
                                     net::stream_file::flags::read_only);
        buffer.resize(file.size());

        net::read(file, net::buffer(buffer));
    }

    const std::vector<char> &get() { return buffer; }

    std::vector<char> buffer;
};

class session : public std::enable_shared_from_this<session> {
  public:
    session(net::io_context *io_context, tcp::socket socket,
            ReadmeContent *content)
        : io_context_(io_context), socket_(std::move(socket)), sbuf{max_length},
          content_{content} {}

    void start() { do_read(); }

  private:
    void do_read() {
        auto self(shared_from_this());

        net::async_read_until(
            socket_, sbuf, "\r\n\r\n",
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    std::cout << "Received " << length << " bytes\n";
                    sbuf.consume(length);
                    do_write();
                } else if (ec == net::error::eof) {
                    std::cout << "Connection closed by peer.\n";
                    return;
                } else {
                    std::cerr << "Boost System Error: " << ec << "\n";
                    return;
                }
            });
    }

    void do_write() {
        auto self(shared_from_this());

        constexpr auto header_template =
            "HTTP/1.1 200 OK\r\n"
            "Server: nginx/1.29.4\r\n"
            "Date: Sat, 03 Jan 2026 15:22:53 GMT\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: {}\r\n"
            "Last-Modified: Sun, 28 Dec 2025 17:25:28 GMT\r\n"
            "Connection: keep-alive\r\n"
            "ETag: \"69516808-4a8\"\r\n"
            "Accept-Ranges: bytes\r\n"
            "\r\n"sv;

        auto buffer = content_->get();
        auto header = std::format(header_template, buffer.size());
        net::async_write(
            socket_,
            std::array<net::const_buffer, 2>{
                net::buffer(header.data(), header.size()),
                net::buffer(buffer.data(), buffer.size())},
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    std::cout << "Sent " << length << " bytes\n";
                    do_read();
                } else if (ec == net::error::eof) {
                    std::cout << "Connection closed by peer.\n";
                    return;
                } else {
                    std::cerr << "Boost System Error: " << ec << "\n";
                    return;
                }
            });
    }

    net::io_context *io_context_;
    tcp::socket socket_;
    net::streambuf sbuf;
    ReadmeContent *content_;
};

class server {
  public:
    server(boost::asio::io_context &io_context, short port)
        : io_context_(&io_context),
          acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
          content_{*io_context_} {
        do_accept();
    }

  private:
    void do_accept() {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::make_shared<session>(io_context_, std::move(socket),
                                              &content_)
                        ->start();
                }

                do_accept();
            });
    }

    net::io_context *io_context_;
    tcp::acceptor acceptor_;
    ReadmeContent content_;
};

int main(int argc, char *argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: blocking_tcp_echo_server <port>\n";
            return 1;
        }

        net::io_context io_context;

        server s(io_context, std::atoi(argv[1]));

        std::vector<std::thread> threads;
        auto thread_count = std::thread::hardware_concurrency();
        for (unsigned i = 0; i < thread_count; ++i) {
            threads.emplace_back([&io_context]() { io_context.run(); });
        }
        for (auto &t : threads)
            t.join();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}