#include <boost/asio.hpp>
#include <boost/asio/stream_file.hpp>
#include <cstdlib>
#include <format>
#include <iostream>
#include <string_view>
#include <thread>
#include <utility>

namespace net = boost::asio;
using net::awaitable;
using net::co_spawn;
using net::detached;
using net::use_awaitable;
using net::ip::tcp;
namespace this_coro = boost::asio::this_coro;

using namespace std::string_view_literals;

const int max_length = 1024;

template <typename Executor> struct ReadmeContent {

    ReadmeContent(Executor &executor) {
        auto file = net::stream_file(executor, "README.md",
                                     net::stream_file::flags::read_only);
        buffer.resize(file.size());

        net::read(file, net::buffer(buffer));
    }

    const std::vector<char> &get() { return buffer; }

    std::vector<char> buffer;
};

template <typename Executor>
awaitable<void> session(tcp::socket sock, ReadmeContent<Executor> &content) {
    try {
        auto sbuf = net::streambuf(max_length);
        for (;;) {
            auto len = co_await net::async_read_until(sock, sbuf, "\r\n\r\n",
                                                      use_awaitable);
            std::cout << "Received " << len << " bytes\n";
            sbuf.consume(len);

            try {
                auto executor = co_await this_coro::executor;
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

                auto write_len = co_await net::async_write(
                    sock,
                    std::array<net::const_buffer, 2>{
                        net::buffer(header.data(), header.size()),
                        net::buffer(buffer.data(), buffer.size())},
                    use_awaitable);
                std::cout << "Sent " << write_len << " bytes\n";
            } catch (const boost::system::system_error &e) {
                std::cerr << "Error opening file: " << e.what() << "\n";
                // TODO: Do proper error handling here,
                // e.g., send a 404 Not Found response to the client.
                co_return;
            }
        }
    } catch (boost::system::system_error &e) {
        if (e.code() == net::error::eof) {
            std::cout << "Connection closed by peer.\n";
            co_return;
        } else {
            std::cerr << "Boost System Error in thread: " << e.what() << "\n";
            co_return;
        }
    } catch (std::exception &e) {
        std::cerr << "Exception in thread: " << e.what() << "\n";
    }
}

awaitable<void> server(unsigned short port) {
    auto executor = co_await this_coro::executor;
    auto content = ReadmeContent(executor);
    tcp::acceptor a(executor, tcp::endpoint(tcp::v4(), port));
    for (;;) {
        auto socket = co_await a.async_accept(use_awaitable);
        co_spawn(executor, session(std::move(socket), content), detached);
    }
}

int main(int argc, char *argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: blocking_tcp_echo_server <port>\n";
            return 1;
        }

        net::io_context io_context;

        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto) { io_context.stop(); });

        co_spawn(io_context, server(std::atoi(argv[1])), detached);

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