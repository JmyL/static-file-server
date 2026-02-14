#include <boost/asio.hpp>
#include <boost/asio/stream_file.hpp>
#include <cstdlib>
#include <format>
#include <iostream>
#include <string_view>
#include <thread>
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

void session(net::io_context &io_context, tcp::socket sock,
             ReadmeContent &content) {
    try {
        auto sbuf = net::streambuf(max_length);
        for (;;) {
            auto len = net::read_until(sock, sbuf, "\r\n\r\n");
            std::cout << "Received " << len << " bytes\n";
            sbuf.consume(len);

            try {
                // open_file(io_context
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

                auto buffer = content.get();
                auto header = std::format(header_template, buffer.size());

                auto write_len = net::write(
                    sock, std::array<net::const_buffer, 2>{
                              net::buffer(header.data(), header.size()),
                              net::buffer(buffer.data(), buffer.size())});
                std::cout << "Sent " << write_len << " bytes\n";
            } catch (const boost::system::system_error &e) {
                std::cerr << "Error opening file: " << e.what() << "\n";
                // TODO: Do proper error handling here,
                // e.g., send a 404 Not Found response to the client.
                return;
            }
        }
    } catch (boost::system::system_error &e) {
        if (e.code() == net::error::eof) {
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

void server(net::io_context &io_context, unsigned short port) {
    ReadmeContent content{io_context};
    tcp::acceptor a(io_context, tcp::endpoint(tcp::v4(), port));
    for (;;) {
        std::thread(session, std::ref(io_context), a.accept(),
                    std::ref(content))
            .detach();
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